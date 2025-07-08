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
 * process_2_sequencer_pressure.h
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_
#define PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_

#define		INITIAL_NO_PRESSURE			256
#define		INITIAL_PRESSURE_SAMPLES	512
#define		PRESSURE_SAMPLES			1024

#define		PRESSURE_OFFSET				50

typedef struct
{
	uint8_t		operating_mode;
	uint8_t		autorange_value;
	uint16_t	zero_pressure_buffer[INITIAL_PRESSURE_SAMPLES];
	uint16_t	pressure_buffer[PRESSURE_SAMPLES];
	uint32_t	adc_zero_data;
	uint16_t	adc_zero_acqcounter;
	uint32_t	adc_data;
	uint32_t	pressure_buffer_index;
	uint32_t	pressure;
	uint32_t	pressure_buffer_number;
}Presso_SequencerPressure_TypeDef;

#define	OPERATING_MODE_AUTORANGE	0x80

extern	ADC_Drv_TypeDef	adc_Drv;

extern	void pressure_manager(void);
extern	uint32_t get_pressure(void);

#endif /* PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_ */
