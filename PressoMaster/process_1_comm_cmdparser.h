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
 * process_1_comm_cmdparser.h
 *
 *  Created on: Dec 5, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_
#define PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_


#define	CMDPARSER_RET_PRG				1
#define	CMDPARSER_RET_LOADRUN			2
#define	CMDPARSER_RET_LOAD				3
#define	CMDPARSER_RET_RUN				4
#define	CMDPARSER_RET_PAUSE				5
#define	CMDPARSER_RET_UNPAUSE			6
#define	CMDPARSER_RET_SET_GLBTIME		7
#define	CMDPARSER_RET_HLT				8
#define	CMDPARSER_RET_WAV				9
#define	CMDPARSER_RET_PLAY				10
#define	CMDPARSER_RET_MUTE				11
#define	CMDPARSER_PLAY_SOUND			12
#define	CMDPARSER_TEST_MOTOR			13
#define	CMDPARSER_TEST_OPEN				14
#define	CMDPARSER_TEST_CLOSE			15
#define	CMDPARSER_TEST_AUTORANGE		16
#define	CMDPARSER_GET_VERSION			30
#define	CMDPARSER_RET_SETTABLES			31

extern	uint8_t Host_pack_USB_packet(uint8_t *usb_rx_buffer,uint8_t len);
extern	uint8_t parse_packet(uint8_t *rx_buf);

#endif /* PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_ */
