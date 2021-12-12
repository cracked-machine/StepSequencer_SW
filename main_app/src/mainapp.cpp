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

	tlc5955::Driver leds;
	leds.reset();
	leds.set_latch_cmd(true);
	leds.set_function_cmd(true, false, true, true, true);
	leds.set_global_brightness_cmd(0x7F, 0x7F, 0x7F);
	leds.set_max_current_cmd(0x4, 0x4, 0x4);
	leds.set_dot_correction_cmd_all(0x7F);
	leds.process_register();
	
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

		// // set led driver gs data
		leds.reset();
		leds.set_latch_cmd(false);
		leds.set_greyscale_cmd_all(0x7FFF);
		leds.process_register();
		
	}
}



#ifdef __cplusplus
}
#endif
