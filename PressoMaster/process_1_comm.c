/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * process_1_comm.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"

NevolSystem_typedef	NevolSystem;

uint8_t	usb_rx_buffer[USB_RX_BUF_SIZE];
uint8_t	packed_usb_rx_buffer[USB_RX_BUF_SIZE];
uint8_t	usb_tx_buffer[USB_TX_BUF_SIZE];

USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = XMODEM_LINE_LEN,
		.timeout = 250,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;

uint8_t		xmodem_usb_enable_poll;
uint8_t		xmodem_usb_reply;
uint8_t		nak=X_NAK,ack=X_ACK;
uint8_t		usb_buf_len;

uint8_t		xmodem_data_area[xmodem_data_len];
uint8_t		xmodem_timeout = XMODEM_TIMEOUT;
uint8_t		comm_state = COMM_NORMAL_MODE;
uint8_t		presso_flash_error = 0;
uint8_t		parsed_cmd;
uint8_t		comm_to_seq_mbx[2];

int32_t ee_program_write(uint32_t data_len,uint8_t program_audio_number)
{
uint32_t	flash_address , size = sizeof(Presso_ee_TypeDef);
Presso_ee_TypeDef	*pstruct;

	if ( program_audio_number == 0 )
		pstruct = &Presso_opening_ee;
	else
		pstruct = &Presso_ee;

	flash_address = pstruct->program_number * sizeof(Presso_ee_TypeDef);
	if ( flash_address <= i2c_24xx_Drv.device_size - sizeof(Presso_ee_TypeDef))
	{
			if ( i2c_extflash_write(i2cflash_driver_handle, flash_address,(uint8_t *)pstruct,size) == 0  )
				return 0;
	}
	presso_flash_error++;
	return 1;
}

int32_t ee_sound_write(uint32_t data_len,uint8_t program_audio_number)
{
uint32_t	flash_address = PRESSO_SOUND_ADDRESS , size = sizeof(Presso_ee_sound_TypeDef);
Presso_ee_sound_TypeDef	*pstruct;

	pstruct = &Presso_ee_sound;
	if ( pstruct->sound_number < 8 )
	{
		flash_address += (pstruct->sound_number) * size;
		if ( flash_address <= i2c_24xx_Drv.device_size - size)
			return i2c_extflash_write(i2cflash_driver_handle, flash_address,(uint8_t *)pstruct,size);
	}
	presso_flash_error++;
	return 1;
}

extern	uint8_t	app_version[32];
void send_version(void)
{
	sprintf((char *)usb_tx_buffer,"App %s - Os %s",app_version,A_OS_VERSION);
	usb_send(usb_driver_handle,usb_tx_buffer,strlen((char *)usb_tx_buffer));
}

void process_1_comm(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	ndecoded;
uint32_t	file_type;	// 0 for csv , 1 for wav, defaults to csv
uint8_t		program_audio_number;

	xmodem_usb_enable_poll = 0;
	file_type = TRANSFER_XMODEM_CSV;
	xmodem_rx_init((uint8_t *)xmodem_data_area,xmodem_data_len);
	usb_driver_handle = usb_device_driver_register(&USB_Drv);

	create_timer(TIMER_ID_0,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( comm_state == COMM_XMODEM_MODE)
			{
				if ( xmodem_usb_enable_poll == 1 )
				{
					xmodem_rx_set_data_area((uint8_t *)xmodem_data_area,xmodem_data_len );
					usb_send(usb_driver_handle,&nak,1);
				}
				xmodem_timeout--;
				if ( xmodem_timeout == 0 )
				{
					xmodem_usb_enable_poll = 1;
				}
			}
		}

		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if ( comm_state == COMM_XMODEM_MODE)
			{
				xmodem_usb_enable_poll = 0;
				xmodem_timeout = XMODEM_TIMEOUT;
				usb_buf_len = usb_get_rx_len(usb_driver_handle);
				xmodem_usb_reply = xmodem_rx_line_parser(usb_rx_buffer);
				switch(xmodem_usb_reply)
				{
				case	X_EOT:
					if ( file_type == TRANSFER_XMODEM_CSV)
					{
						ndecoded = decode_csv((uint8_t *)xmodem_data_area,xmodem_rx_get_rxed_amount(),&program_audio_number);
						if ( ndecoded )
						{
							if ( ee_program_write(ndecoded,program_audio_number) == 0 )
								usb_send(usb_driver_handle,&ack,1);
							else
								usb_send(usb_driver_handle,&nak,1);
						}
						else
							usb_send(usb_driver_handle,&nak,1);
					}

					if ( file_type == TRANSFER_XMODEM_SOUND_CSV)
					{
						ndecoded = decode_csv((uint8_t *)xmodem_data_area,xmodem_rx_get_rxed_amount(),&program_audio_number);
						if ( ndecoded )
						{
							if ( ee_sound_write(ndecoded,program_audio_number) == 0 )
								usb_send(usb_driver_handle,&ack,1);
							else
								usb_send(usb_driver_handle,&nak,1);
						}
						else
							usb_send(usb_driver_handle,&nak,1);
					}

					if ( file_type == TRANSFER_XMODEM_WAV)
					{
						ndecoded = xmodem_rx_get_rxed_amount();
						if ( wav_flash_to_bank2((uint8_t *)xmodem_data_area,ndecoded,NevolSystem.param_from_host-1) == 0 )
							usb_send(usb_driver_handle,&ack,1);
						else
							usb_send(usb_driver_handle,&nak,1);
					}
					comm_state = COMM_NORMAL_MODE;
					USB_Drv.data_index = 0;
					break;
				case	X_ACK:
					usb_send(usb_driver_handle,&ack,1);
					break;
				default:
					usb_send(usb_driver_handle,&nak,1);
					break;
				}
			}
			if ( comm_state == COMM_NORMAL_MODE )
			{
				usb_buf_len = usb_get_rx_len(usb_driver_handle);

				if ( Host_pack_USB_packet(usb_rx_buffer,usb_buf_len) == 0 )
				{
					parsed_cmd = parse_packet(packed_usb_rx_buffer);
					switch(parsed_cmd)
					{
					case CMDPARSER_RET_PRG:
						file_type = TRANSFER_XMODEM_CSV;
						comm_state = COMM_XMODEM_MODE;
						xmodem_usb_enable_poll = 1;
						break;
					case CMDPARSER_RET_SOUND:
						file_type = TRANSFER_XMODEM_SOUND_CSV;
						comm_state = COMM_XMODEM_MODE;
						xmodem_usb_enable_poll = 1;
						break;
					case CMDPARSER_RET_WAV:
						file_type = TRANSFER_XMODEM_WAV;
						comm_state = COMM_XMODEM_MODE;
						xmodem_usb_enable_poll = 1;
						break;
					case CMDPARSER_RET_LOADRUN:
					case CMDPARSER_RET_LOAD:
					case CMDPARSER_RET_RUN:
					case CMDPARSER_RET_HLT:
					case CMDPARSER_RET_PLAY:
					case CMDPARSER_RET_MUTE:
					case CMDPARSER_PLAY_SOUND:
						comm_to_seq_mbx[0] = parsed_cmd;
						comm_to_seq_mbx[1] = NevolSystem.param_from_host;
						mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_COMM_MBX,comm_to_seq_mbx,2);
						break;
					case CMDPARSER_TEST_MOTOR:
						comm_to_seq_mbx[0] = parsed_cmd;
						comm_to_seq_mbx[1] = NevolSystem.param_from_host;
						mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_COMM_MBX,comm_to_seq_mbx,2);
						break;
					case CMDPARSER_TEST_OPEN:
						comm_to_seq_mbx[0] = parsed_cmd;
						comm_to_seq_mbx[1] = NevolSystem.param_from_host;
						mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_COMM_MBX,comm_to_seq_mbx,2);
						break;
					case CMDPARSER_TEST_CLOSE:
						comm_to_seq_mbx[0] = parsed_cmd;
						comm_to_seq_mbx[1] = NevolSystem.param_from_host;
						mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_COMM_MBX,comm_to_seq_mbx,2);
						break;
					case CMDPARSER_TEST_AUTORANGE:
						comm_to_seq_mbx[0] = parsed_cmd;
						comm_to_seq_mbx[1] = NevolSystem.param_from_host;
						mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_COMM_MBX,comm_to_seq_mbx,2);
						break;
					case CMDPARSER_GET_VERSION:
						send_version();
						break;

					}
				}
			}
		}
	}
}
