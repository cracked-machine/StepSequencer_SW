/*
 * mainapp.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#include "mainapp.hpp"
#include <ssd1306.hpp>
#include <tlc5955.hpp>
#include <chrono>
#include <thread>

#include <sstream>

#ifdef __cplusplus
extern "C"
{
#endif

bool tlc5955_callback {false};

#ifdef USE_HAL_DRIVER
#include <stm32g0xx_hal.h>
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);      
	//HAL_SPI_DMAPause(&hspi2);

    // disable the gsclk
    HAL_GPIO_WritePin(TLC5955_SPI2_GSCLK_GPIO_Port, TLC5955_SPI2_GSCLK_Pin, GPIO_PIN_RESET);
    
    // toggle the latch pin
    //HAL_Delay(1);
    HAL_GPIO_WritePin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin, GPIO_PIN_SET);
    //HAL_Delay(1);
    HAL_GPIO_WritePin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin, GPIO_PIN_RESET);
    //HAL_Delay(1);

    // enable the gsclk
    HAL_GPIO_WritePin(TLC5955_SPI2_GSCLK_GPIO_Port, TLC5955_SPI2_GSCLK_Pin, GPIO_PIN_SET);	
	
	//HAL_SPI_DMAResume(&hspi2);
}    
#endif	

void mainapp()
{

	std::bitset<tlc5955::Driver::m_latch_size_bits> ctrl_latch_cmd {0b1};
	std::bitset<tlc5955::Driver::m_latch_size_bits> gs_latch_cmd {0b0};
	std::bitset<tlc5955::Driver::m_ctrl_cmd_size_bits> ctrl_cmd {0x96};
	std::bitset<tlc5955::Driver::m_padding_section_size_bits> padding {0x00};
	std::bitset<tlc5955::Driver::m_func_data_section_size_bits> function_cmd {0b10111};
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc {0b000'0001};	
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc_on {0b111'1111};	
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc_off {0b000'0000};	
	std::bitset<tlc5955::Driver::m_mc_data_resolution> led_mc {0b100};		
	std::bitset<tlc5955::Driver::m_dc_data_resolution> led_dc {0x7F};		
	std::bitset<tlc5955::Driver::m_gs_data_resolution> led_gs {0x7FFF};

	tlc5955::Driver leds;
	leds.m_common_bit_register.reset();
	leds.add_bitset(leds.m_common_bit_register, ctrl_latch_cmd, tlc5955::Driver::m_latch_offset);
	leds.add_bitset(leds.m_common_bit_register, ctrl_cmd, tlc5955::Driver::m_ctrl_cmd_offset);
	leds.add_bitset(leds.m_common_bit_register, padding, tlc5955::Driver::m_padding_offset);
	leds.add_bitset(leds.m_common_bit_register, function_cmd, tlc5955::Driver::m_func_data_offset);
	// BC data
	leds.add_bitset(leds.m_common_bit_register, led_bc_on, tlc5955::Driver::m_bc_data_offset);
	leds.add_bitset(leds.m_common_bit_register, led_bc_on, tlc5955::Driver::m_bc_data_offset + tlc5955::Driver::m_bc_data_resolution);
	leds.add_bitset(leds.m_common_bit_register, led_bc_on, tlc5955::Driver::m_bc_data_offset + tlc5955::Driver::m_bc_data_resolution * 2);
	// MC data
	leds.add_bitset(leds.m_common_bit_register, led_mc, tlc5955::Driver::m_mc_data_offset);
	leds.add_bitset(leds.m_common_bit_register, led_mc, tlc5955::Driver::m_mc_data_offset + tlc5955::Driver::m_mc_data_resolution);
	leds.add_bitset(leds.m_common_bit_register, led_mc, tlc5955::Driver::m_mc_data_offset + tlc5955::Driver::m_mc_data_resolution * 2);	
	// DC data
	for (uint8_t dc_idx = 0; dc_idx < 48; dc_idx++)
	{
		leds.add_bitset(leds.m_common_bit_register, led_dc, tlc5955::Driver::m_dc_data_offset + tlc5955::Driver::m_dc_data_resolution * dc_idx);		
	}

	leds.bitset_to_bytearray( leds.m_common_byte_register, leds.m_common_bit_register);
	leds.print_bits(leds.m_common_bit_register); 

	// enable the gsclk
	#ifdef USE_HAL_DRIVER
    	HAL_GPIO_WritePin(TLC5955_SPI2_GSCLK_GPIO_Port, TLC5955_SPI2_GSCLK_Pin, GPIO_PIN_SET);
		leds.start_dma_transmit();
	#endif


	static ssd1306::Font5x7 font;
	static ssd1306::Display oled;
	oled.init();

	uint8_t count = 0;
	//uint32_t delay_ms {0};
	while(true)
	{
		// run oled animation
		std::stringstream msg;
		msg << font.character_map[count];
		oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
		if (count < font.character_map.size() - 1) { count++; }
		else { count = 0; }		

		// set led driver gs data
		leds.m_common_bit_register.reset();
		leds.add_bitset(leds.m_common_bit_register, gs_latch_cmd, tlc5955::Driver::m_latch_offset);
		for (uint16_t gs_idx = 0; gs_idx < 48; gs_idx++)
		{
			leds.add_bitset(leds.m_common_bit_register, led_gs, tlc5955::Driver::m_gs_data_offset + tlc5955::Driver::m_gs_data_resolution * gs_idx);
		}

		leds.bitset_to_bytearray( leds.m_common_byte_register, leds.m_common_bit_register);
		leds.print_bits(leds.m_common_bit_register); 
		
	}
}



#ifdef __cplusplus
}
#endif
