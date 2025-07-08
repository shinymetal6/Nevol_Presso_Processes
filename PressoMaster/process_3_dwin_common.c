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
 * process_3_dwin_common.c
 *
 *  Created on: Apr 15, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"


DWIN_packet_typedef					DWIN_packet;
extern	DWIN_sm_typedef				DWIN_sm;
extern	Presso_Sequencer_TypeDef	Presso_Sequencer;

static void dwinsend(uint32_t uart_driver_handle,uint32_t pktlen)
{
	uart_send(uart_driver_handle, (uint8_t *)&DWIN_packet,pktlen);
}

uint32_t compile_and_send_7b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint32_t data)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x07;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = (uint8_t )address>>8;
	DWIN_packet.address_l = (uint8_t )address;
	DWIN_packet.data0_h = (uint8_t )(data>>24);
	DWIN_packet.data0_l = (uint8_t )(data>>16);
	DWIN_packet.data1_h = (uint8_t )(data>>8);
	DWIN_packet.data1_l = (uint8_t )(data);
	dwinsend(uart_driver_handle,10);
	return 0;
}

uint32_t compile_and_send_5b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint16_t data)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x05;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = address>>8;
	DWIN_packet.address_l = address;
	DWIN_packet.data0_h = (uint8_t )(data>>8);
	DWIN_packet.data0_l = (uint8_t )(data);
	dwinsend(uart_driver_handle,8);
	task_delay(10);
	return 0;
}

uint32_t compile_and_send_5b_dwin_packet_nowait(uint32_t uart_driver_handle,uint16_t address,uint16_t data)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x05;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = address>>8;
	DWIN_packet.address_l = address;
	DWIN_packet.data0_h = (uint8_t )(data>>8);
	DWIN_packet.data0_l = (uint8_t )(data);
	dwinsend(uart_driver_handle,8);
	return 0;
}

void compile_and_send_text_dwin(uint32_t uart_driver_handle,const char* text, uint16_t address)
{
uint32_t	i,bcount;
int 		textLen;

	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x13;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = address>>8;
	DWIN_packet.address_l = address;

    char *ptr = (char *)&DWIN_packet.data0_h;

    memset(ptr,0x20,16);
    if (strcmp(text, "Clear@") == 0)
    {
    	ptr[0] = '-';
    	ptr[1] = '-';
    	ptr[2] = ':';
    	ptr[3] = '-';
    	ptr[4] = '-';
    	ptr[5] = 0;
        bcount = strlen(ptr)+6;
    }
    else
    {
        textLen = strlen(text);
        for (i = 0; i < textLen; i++)
        	ptr[i] = text[i];
        bcount = strlen(ptr)+6+1;
    }
    DWIN_packet.number_of_bytes = bcount;
	dwinsend(uart_driver_handle,bcount);
	task_delay(10);
	dwinsend(uart_driver_handle,bcount);
	task_delay(10);
}

void dwin_update_fields(uint32_t uart_driver_handle,Presso_ee_TypeDef *current_presso_ee)
{
uint32_t	i;
int	minutes,seconds;
char prgtime[16];

	for(i=0;i<8;i++)
	{
		compile_and_send_5b_dwin_packet(uart_driver_handle,PRESSURE_BASE_ADDRESS+(i*0x10),current_presso_ee->Presso_ee_line->sector_pressure);
		compile_and_send_5b_dwin_packet(uart_driver_handle,TSECTOR_BASE_ADDRESS+(i*0x10),current_presso_ee->program_step_time);
	}
	minutes = Presso_ee.program_complete_time / 60;
	seconds = Presso_ee.program_complete_time - ( minutes * 60);
	sprintf(prgtime,"%d:%d",minutes,seconds);
	compile_and_send_text_dwin(uart_driver_handle,prgtime,TOTAL_TIME_ADDRESS);
}

uint32_t dwin_update_total_time(uint32_t uart_driver_handle)
{
char prgtime[16];
int	minutes,seconds;

	minutes = Presso_ee.program_complete_time / 60;
	seconds = Presso_ee.program_complete_time - ( minutes * 60);
	if ( seconds < 10 )
		sprintf(prgtime,"%d:%02d",minutes,seconds);
	else
		sprintf(prgtime,"%d:%d",minutes,seconds);
	compile_and_send_text_dwin(uart_driver_handle,prgtime,TOTAL_TIME_ADDRESS);
	return 0;
}

uint32_t dwin_update_current_time(uint32_t uart_driver_handle)
{
char prgtime[16];
int	minutes,seconds;

	minutes = Presso_Sequencer.cycle_time / 60;
	seconds = Presso_Sequencer.cycle_time - ( minutes * 60);
	if ( seconds < 10 )
		sprintf(prgtime,"%d:%02d",minutes,seconds);
	else
		sprintf(prgtime,"%d:%d",minutes,seconds);
	compile_and_send_text_dwin(uart_driver_handle,prgtime,TOTAL_TIME_ADDRESS);
	return 0;
}

void dwin_clear_fields(uint32_t uart_driver_handle)
{
uint32_t	i;
	for(i=0;i<8;i++)
	{
		compile_and_send_5b_dwin_packet(uart_driver_handle,PRESSURE_BASE_ADDRESS+(i*0x10),0);
		compile_and_send_5b_dwin_packet(uart_driver_handle,TSECTOR_BASE_ADDRESS+(i*0x10),0);
	}
	compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
	compile_and_send_text_dwin(uart_driver_handle,"Clear@",TOTAL_TIME_ADDRESS);
}



