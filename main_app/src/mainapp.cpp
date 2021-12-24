// MIT License

// Copyright (c) 2021 Chris Sutton

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "mainapp.hpp"

#include <ssd1306.hpp>
#include <tlc5955.hpp>

#include <ll_tim_utils.hpp>
#include <chrono>
#include <thread>
#include <sstream>

#ifdef __cplusplus
extern "C"
{
#endif

bool tlc5955_callback {false};

#ifdef USE_TLC5955_HAL_DRIVER
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

	// refresh buffers
	leds.reset();
	// construct the SPI transmit data
	leds.set_ctrl_cmd();
	leds.set_padding_bits();
	leds.set_function_cmd(true, true, true, false, true);
	leds.set_global_brightness_cmd(0x7F, 0x7F, 0x7F);
	leds.set_max_current_cmd(0x5, 0x5, 0x5);
	leds.set_dot_correction_cmd_all(0x01);
	// prepare SPI transmit data as bytes
	leds.process_register();
	// send the data
	leds.toggle_latch(true);
	leds.send_blocking_transmit();

	static ssd1306::Font5x7 font;
	static ssd1306::Font16x26 font2;
	static ssd1306::Display oled;

	oled.init();
	uint8_t count = 0;

	// set the max freq (16MHz) for PWM when TIM4_ARR is also 1.
	LL_TIM_OC_SetCompareCH1(TIM4, 0x1);
	// enable the timer but not the output
	LL_TIM_EnableCounter(TIM4);    	

	//uint32_t delay_ms {0};
	while(true)
	{

		// refresh buffers
		leds.reset();
		// construct the SPI transmit data
		leds.set_greyscale_cmd_all(0x0000);
		// prepare SPI transmit data as bytes
		leds.process_register();
		// send the data
		leds.toggle_latch(false);
		leds.send_blocking_transmit();
	
	// send the control data
	// leds.send_blocking_transmit();

		// // turn all LEDs off
		// leds.reset();
		// leds.set_latch_cmd(false);
		// leds.set_greyscale_cmd_all(0x0000);
		// leds.process_register();
		// leds.send_blocking_transmit();
		
		// run oled animation		
		std::string msg {font2.character_map[count]};
		oled.write(msg, font2, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
		if (count < font2.character_map.size() - 1) { count++; }
		else { count = 0; }		

		#ifdef USE_FULL_LL_DRIVER
			embed_utils::tim::ll_delay_microsecond(TIM14, 0x01);
			// LL_mDelay(100);
		#endif

		// refresh buffers
		leds.reset();
		// construct the SPI transmit data
		leds.set_greyscale_cmd_all(0x7FFF);
		// prepare SPI transmit data as bytes
		leds.process_register();
		// send the data
		leds.toggle_latch(false);
		leds.send_blocking_transmit();

		// // turn all LEDs on
		// leds.reset();
		// leds.set_latch_cmd(false);
		// leds.set_greyscale_cmd_all(0x7FFF);
		// leds.process_register();
		// leds.send_blocking_transmit();
		
		#ifdef USE_FULL_LL_DRIVER
			embed_utils::tim::ll_delay_microsecond(TIM14, 0x01);
			// LL_mDelay(100);
		#endif
	}
}



#ifdef __cplusplus
}
#endif
