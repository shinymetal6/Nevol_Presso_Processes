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
 * process_2_sequencer_init.c
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_2_sequencer.h"

extern	I2C_HandleTypeDef hi2c1;
extern	Presso_SequencerPressure_TypeDef	Presso_SequencerPressure;


uint8_t	i2cBufw[PRESSO_BUFFERSIZE];
uint8_t	i2cBufr[PRESSO_BUFFERSIZE];

ExtFlash_DriverStruct_t	i2c_24xx_driver;

I2C_24xx_Drv_TypeDef	i2c_24xx_Drv =
{
		.bus = &hi2c1,
		.i2c_scl_port = GPIOB,
		.i2c_scl_bit = 8,
		.device_address = I2C_24XX_ADDRESS,
		.device_address_size = I2C_MEMADD_SIZE_16BIT,
		.device_size = 65536,
		.flags = I2C_FLAGS_USES_READ_DMA | I2C_FLAGS_USES_WRITE_DMA | I2C_FLAGS_WAKEUP_ON_READ | I2C_FLAGS_WAKEUP_ON_WRITE | I2C_FLAGS_WAIT_ON_WRITE_COMPLETE | I2C_FLAGS_WAIT_ON_READ_COMPLETE,
		.wakeup_id = WAKEUP_FROM_I2C1_IRQ,
};
uint32_t		i2cflash_driver_handle;

extern	ADC_HandleTypeDef hadc1;
extern	TIM_HandleTypeDef htim6;

uint16_t	adcBuf[4];
/*
 * 0.5  = 	1733
 *
 */

ADC_Drv_TypeDef	adc_Drv =
{
		.adc = &hadc1,
		.adc_timer = &htim6,
		.adc_buffer = adcBuf,
		.num_channels = 3,
		.wakeup_id = EVENT_ADC1_IRQ,
		.flags = ADC_FLAGS_ALL_WAKEUP,
};
uint32_t		adc_driver_handle;

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim7;

int16_t	out_buffer[NUMBER_OF_AUDIO_SAMPLES];

DAC_Drv_TypeDef	dac_Drv =
{
		.dac = &hdac1,
		.dac_timer = &htim7,
		.channel = DAC_CHANNEL_1,
		.dac_buffer = out_buffer,
		.len = NUMBER_OF_AUDIO_SAMPLES,
		.alignment = DAC_ALIGN_12B_L,
		.flags = DAC_FLAGS_USE_SYNTHMODULE,
};
uint32_t		dac_driver_handle;

MidiSynth_TypeDef Audio_Synth =
{
		.status = SYNTH_ENABLED,
		.out_buf = out_buffer,
		.out_device = SYNTH_DAC_OUT,
		.wavetable_size = SYNTH_WAVETABLE_1024,
};

extern	TIM_HandleTypeDef htim2;
extern	TIM_HandleTypeDef htim3;

Pwm_Control_TypeDef	Pwm2_3_Control =
{
		.pwm_timer = &htim2,
		.pwm_channel = TIM_CHANNEL_3,
};
uint32_t		tim2_3_driver_handle;

Pwm_Control_TypeDef	Pwm3_1_Control =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_1,
};
uint32_t		tim3_1_driver_handle;

Pwm_Control_TypeDef	Pwm3_2_Control =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_2,
};
uint32_t		tim3_2_driver_handle;

Pwm_Control_TypeDef	Pwm3_3_Control =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_3,
};
uint32_t		tim3_3_driver_handle;

Pwm_Control_TypeDef	Pwm3_4_Control =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_4,
};
uint32_t		tim3_4_driver_handle;

Presso_GPIO_TypeDef	Presso_GPIO[16] =
{
		{
				.port = OUT0_GPIO_Port,
				.bit  = OUT0_Pin,
		},
		{
				.port = OUT1_GPIO_Port,
				.bit  = OUT1_Pin,
		},
		{
				.port = OUT2_GPIO_Port,
				.bit  = OUT2_Pin,
		},
		{
				.port = OUT3_GPIO_Port,
				.bit  = OUT3_Pin,
		},
		{
				.port = OUT4_GPIO_Port,
				.bit  = OUT4_Pin,
		},
		{
				.port = OUT5_GPIO_Port,
				.bit  = OUT5_Pin,
		},
		{
				.port = OUT6_GPIO_Port,
				.bit  = OUT6_Pin,
		},
		{
				.port = OUT7_GPIO_Port,
				.bit  = OUT7_Pin,
		},
		{
				.port = OUT8_GPIO_Port,
				.bit  = OUT8_Pin,
		},
		{
				.port = OUT9_GPIO_Port,
				.bit  = OUT9_Pin,
		},
		{
				.port = OUT10_GPIO_Port,
				.bit  = OUT10_Pin,
		},
		{
				.port = OUT11_GPIO_Port,
				.bit  = OUT11_Pin,
		},
		{
				.port = OUT12_GPIO_Port,
				.bit  = OUT12_Pin,
		},
		{
				.port = OUT13_GPIO_Port,
				.bit  = OUT13_Pin,
		},
		{
				.port = OUT14_GPIO_Port,
				.bit  = OUT14_Pin,
		},
		{
				.port = OUT15_GPIO_Port,
				.bit  = OUT15_Pin,
		},
};


int16_t	passthrough_in_buffer2[HALF_NUMBER_OF_AUDIO_SAMPLES];

PASSTHROUGH_Effect_TypeDef	passthrough_private2 =
{
		.call_counter = 0,
};

Effect_TypeDef passthrough2 =
{
		.status = SOUND_EFFECT_ENABLED,
		.in_buf = passthrough_in_buffer2,
		.private_data = (uint32_t *)&passthrough_private2,
		.effect = Effect_Passthrough,
};

int16_t	vca_in_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

VCA_Effect_TypeDef	vca_private =
{
		.amplitude = 50,
};

Effect_TypeDef	VCA =
{
		.status = SOUND_EFFECT_ENABLED,
		.in_buf = vca_in_buffer,
		.private_data = (uint32_t *)&vca_private,
		.effect = Effect_VCA,
};

int16_t	phaser_in_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

PHASER_Effect_TypeDef	phaser_private =
{
		.lfo_rate = PHASER_DEFAULT_LFO_RATE,
		.depth = PHASER_DEFAULT_DEPTH,
		.mix = PHASER_DEFAULT_MIX,
		.allpass_number = PHASER_NUM_ALLPASS,
};

Effect_TypeDef	PHASER =
{
		.status = SOUND_EFFECT_ENABLED,
		.in_buf = phaser_in_buffer,
		.private_data = (uint32_t *)&phaser_private,
		.effect = Effect_Phaser,
		.effect_init = Effect_Phaser_Init,
};

void process_2_sequencer_init(void)
{
uint32_t	i;

	for(i=0;i<PRESSO_BUFFERSIZE;i++)
		i2cBufw[i] = ( i +1 ) & 0xff;
	for(i=0;i<PRESSO_GPIO_NR;i++)
		  HAL_GPIO_WritePin(Presso_GPIO[i].port, Presso_GPIO[i].bit, GPIO_PIN_RESET);

	i2cflash_driver_handle = i2c_24xx_register(&i2c_24xx_Drv);

	tim2_3_driver_handle = pwm_register(&Pwm2_3_Control);
	tim3_1_driver_handle = pwm_register(&Pwm3_1_Control);
	tim3_2_driver_handle = pwm_register(&Pwm3_2_Control);
	tim3_3_driver_handle = pwm_register(&Pwm3_3_Control);
	tim3_4_driver_handle = pwm_register(&Pwm3_4_Control);

	pwm_start(tim2_3_driver_handle);
	pwm_start(tim3_1_driver_handle);
	pwm_start(tim3_2_driver_handle);
	pwm_start(tim3_3_driver_handle);
	pwm_start(tim3_4_driver_handle);

	adc_driver_handle = int_adc_register(&adc_Drv);
	adc_start(adc_driver_handle);

	dac_driver_handle = int_dac_register(&dac_Drv);
	dac_init(dac_driver_handle);

	Synth_Init(&Audio_Synth);
	Sound_Insert_Effect(&Audio_Synth,&PHASER);
	Synth_Start(&Audio_Synth);

}

void process_2_sequencer_set_gpio(uint16_t outconfig,uint16_t outconfig_override)
{
uint8_t i;
	for(i=0;i<PRESSO_GPIO_NR;i++)
	{
		if ( ( outconfig & (1 << i ) ) || ( outconfig_override & (1 << i ) ))
			HAL_GPIO_WritePin(Presso_GPIO[i].port, Presso_GPIO[i].bit, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(Presso_GPIO[i].port, Presso_GPIO[i].bit, GPIO_PIN_RESET);
	}
}

void process_2_sequencer_set_timers(uint32_t ht1,uint32_t ht2,uint32_t ht3,uint32_t ht4,uint32_t ht5)
{
	pwm_set_width(tim2_3_driver_handle,ht1*100);
	pwm_set_width(tim3_1_driver_handle,ht2*100);
	pwm_set_width(tim3_2_driver_handle,ht3*100);
	pwm_set_width(tim3_3_driver_handle,ht4*100);
	pwm_set_width(tim3_4_driver_handle,ht5*100);
}

void process_2_sequencer_set_motor(uint8_t motor)
{
	if ( motor )
		HAL_GPIO_WritePin(MOTOR_ON_GPIO_Port, MOTOR_ON_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(MOTOR_ON_GPIO_Port, MOTOR_ON_Pin, GPIO_PIN_RESET);
}

void process_2_sequencer_test_set_gpio(uint8_t gpio_number)
{
	HAL_GPIO_WritePin(Presso_GPIO[gpio_number].port, Presso_GPIO[gpio_number].bit, GPIO_PIN_SET);
}

void process_2_sequencer_test_unset_gpio(uint8_t gpio_number)
{
	HAL_GPIO_WritePin(Presso_GPIO[gpio_number].port, Presso_GPIO[gpio_number].bit, GPIO_PIN_RESET);
}

void process_2_sequencer_test_autorange(uint8_t value)
{
	if ( value )
	{
		Presso_SequencerPressure.operating_mode |= OPERATING_MODE_AUTORANGE;
		Presso_SequencerPressure.autorange_value = value;
	}
	else
		Presso_SequencerPressure.operating_mode &= ~OPERATING_MODE_AUTORANGE;
}

