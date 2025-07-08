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
 * process_3_dwin_common.h
 *
 *  Created on: Apr 15, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_PROCESS_3_DWIN_COMMON_H_
#define PRESSOMASTER_PROCESS_3_DWIN_COMMON_H_

#define	DWIN_PKT_MAX_LEN	4

#define	DWIN_PKT_QUEUE_LEN	8


typedef struct
{
	uint8_t				start_flag_1;
	uint8_t				start_flag_2;
	uint8_t				number_of_bytes;
	uint8_t				command;
	uint8_t				address_h;
	uint8_t				address_l;
	uint8_t				data0_h;
	uint8_t				data0_l;
	uint8_t				data1_h;
	uint8_t				data1_l;
	uint8_t				additional_data[16];
}DWIN_packet_typedef;

#define	HMI_HEADER1		0x5a
#define	HMI_HEADER2		0xa5
#define	HMI_WRITE_CMD	0x82
#define	HMI_READ_CMD	0x83

#define	DWIN_KBD_OK		0xf1
#define	DWIN_KBD_BACK	0xf0

#define	PLAY_PAUSE_BTN_ADDR		0x1950
#define	PLAY_PAUSE_BTN_PLAY		0x0003
#define	PLAY_PAUSE_BTN_PAUSE	0x0001

#define	PRESSURE_BASE_ADDRESS	0x0400
#define	TSECTOR_BASE_ADDRESS	0x0300
#define	TOTAL_TIME_ADDRESS		0x1900

/*
#define	HIGHLIGHT_GRAY			0x4000
#define	HIGHLIGHT_CYAN			0x5000
#define	HIGHLIGHT_RED			0x1000
#define	HIGHLIGHT_GREEN			0x2000
*/
extern	DWIN_packet_typedef			DWIN_packet;
extern	NevolSystem_typedef			NevolSystem;

extern	uint32_t 	process_from_dwin(uint32_t uart1_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen);
extern	uint32_t 	compile_and_send_7b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint32_t data);
extern	uint32_t 	compile_and_send_5b_dwin_packet_nowait(uint32_t uart_driver_handle,uint16_t address,uint16_t data);
extern	uint32_t 	compile_and_send_5b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint16_t data);
extern	void 		compile_and_send_text_dwin(uint32_t uart_driver_handle,const char* text, uint16_t address);

extern	void 	 	dwin_update_fields(uint32_t uart_driver_handle, Presso_ee_TypeDef *current_presso_ee);
extern	void 	 	dwin_clear_fields(uint32_t uart_driver_handle);
extern	uint32_t 	dwin_state_machine_reset (void);
extern	uint32_t 	dwin_highlight_field(uint32_t uart_driver_handle,uint16_t field);
extern	uint32_t 	dwin_update_total_time(uint32_t uart_driver_handle);
extern	uint32_t 	dwin_update_current_time(uint32_t uart_driver_handle);


#endif /* PRESSOMASTER_PROCESS_3_DWIN_COMMON_H_ */
