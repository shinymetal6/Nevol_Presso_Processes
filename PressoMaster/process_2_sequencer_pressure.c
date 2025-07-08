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
 * process_2_sequencer_pressure.c
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"

extern	Presso_SequencerPressure_TypeDef	Presso_SequencerPressure;
uint32_t adc_errors = 0;
/*
13068 = 300 mBar
12732 = open

 */
void pressure_manager(void)
{
uint32_t	i;
	if ( Presso_SequencerPressure.adc_zero_acqcounter < INITIAL_NO_PRESSURE )
	{
		Presso_SequencerPressure.adc_zero_acqcounter++;
	}
	else if (( Presso_SequencerPressure.adc_zero_acqcounter >= INITIAL_NO_PRESSURE ) && ( Presso_SequencerPressure.adc_zero_acqcounter < INITIAL_PRESSURE_SAMPLES ))
	{
		Presso_SequencerPressure.pressure_buffer[Presso_SequencerPressure.adc_zero_acqcounter-INITIAL_NO_PRESSURE] = adc_Drv.adc_buffer[0];
		Presso_SequencerPressure.adc_zero_acqcounter++;
	}
	else if ( Presso_SequencerPressure.adc_zero_acqcounter == INITIAL_PRESSURE_SAMPLES )
	{
		Presso_SequencerPressure.adc_zero_data = 0;
		for(i=0;i<INITIAL_PRESSURE_SAMPLES-INITIAL_NO_PRESSURE;i++)
			Presso_SequencerPressure.adc_zero_data += Presso_SequencerPressure.pressure_buffer[i];
		Presso_SequencerPressure.adc_zero_data /= (INITIAL_PRESSURE_SAMPLES-INITIAL_NO_PRESSURE);
		for(i=0;i<PRESSURE_SAMPLES;i++)
			Presso_SequencerPressure.pressure_buffer[i] = 0;
		Presso_SequencerPressure.adc_zero_acqcounter = INITIAL_PRESSURE_SAMPLES + 1;
		Presso_SequencerPressure.pressure_buffer_index = 0;
	}
	else
	{
		Presso_SequencerPressure.pressure_buffer[Presso_SequencerPressure.pressure_buffer_index] = adc_Drv.adc_buffer[0];
		Presso_SequencerPressure.pressure_buffer_index++;
		Presso_SequencerPressure.pressure_buffer_index &= (PRESSURE_SAMPLES-1);
		if ( Presso_SequencerPressure.pressure_buffer_index == 0 )
		{
			Presso_SequencerPressure.pressure = 0;
			for(i=0;i<PRESSURE_SAMPLES;i++)
			{
				if ( Presso_SequencerPressure.pressure_buffer[i] >= Presso_SequencerPressure.adc_zero_data)
					Presso_SequencerPressure.pressure += (Presso_SequencerPressure.pressure_buffer[i] - Presso_SequencerPressure.adc_zero_data);
			}
			Presso_SequencerPressure.pressure /= PRESSURE_SAMPLES;
			if ( Presso_SequencerPressure.pressure > PRESSURE_OFFSET)
				Presso_SequencerPressure.pressure -= PRESSURE_OFFSET;
			Presso_SequencerPressure.pressure_buffer_number++;
			for(i=0;i<PRESSURE_SAMPLES;i++)
				Presso_SequencerPressure.pressure_buffer[i] = 0;
			if ( (Presso_SequencerPressure.operating_mode & OPERATING_MODE_AUTORANGE ) == OPERATING_MODE_AUTORANGE)
			{
				if ( Presso_SequencerPressure.pressure > Presso_SequencerPressure.autorange_value )
					process_2_sequencer_set_motor(0);
			}
		}
	}
}

uint32_t get_pressure(void)
{
	return Presso_SequencerPressure.pressure;
}

