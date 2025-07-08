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
 * process_2_sequencer_memparser.h
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#ifndef PROCESS_2_SEQUENCER_MEMPARSER_H_
#define PROCESS_2_SEQUENCER_MEMPARSER_H_

#define	PRESSO_FLASH_SIZE	65536

typedef struct
{
	uint8_t		line_number;		//1
	uint8_t		audionumber;		//1
	uint8_t		flags;				//1
	uint8_t		heater_values[5];	//5
	uint16_t	gpio;				//2
	uint8_t		sector_time;		//1
	uint8_t		sector_pressure;	//1
	uint8_t		line_valid;			//1
	uint8_t		unused[3];			//3
}Presso_ee_line_TypeDef;
#define	PRESSO_LINE_LOADED		0x7e

/* the program header is 32 bytes long
 * a program line is 16 bytes long
 * there are 30 lines per program
 * size must be 512
*/
#define	EE_PROG_NAME_SIZE		21
#define	EE_MAX_LINE_NUMBER		30

typedef struct
{
	uint8_t					program_valid_flag;			// 1
	uint8_t					program_number;				// 1
	uint8_t					program_number_of_lines;	// 1
	uint8_t					program_has_opening;		// 1
	uint8_t					program_close_eoc;			// 1
	uint16_t				program_step_time;			// 2
	uint16_t				program_complete_time;		// 2
	char					program_name[EE_PROG_NAME_SIZE];
	Presso_ee_line_TypeDef	Presso_ee_line[EE_MAX_LINE_NUMBER];	//4
}Presso_ee_TypeDef;
#define	EE_PROG_VALID_FLAG		0xe7
#define	EE_PROG_HDR_SIZE		32
#define	EE_PROG_LINE_SIZE		16

extern	uint32_t mem_load_program(uint8_t program_number);

#endif /* PROCESS_2_SEQUENCER_MEMPARSER_H_ */
