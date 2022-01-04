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

#ifdef USE_FULL_LL_DRIVER
	#include <ll_tim_utils.hpp>
#endif

#include <chrono>
#include <thread>
#include <sstream>

#ifdef __cplusplus
extern "C"
{
#endif


tlc5955::Driver leds;

void setup_leds()
{
	// refresh buffers
	leds.reset();
	// construct the SPI transmit data
	leds.set_ctrl_cmd();
	leds.set_padding_bits();
	leds.set_function_cmd(true, true, true, false, true);
	leds.set_global_brightness_cmd(0x7F, 0x7F, 0x7F);
	leds.set_max_current_cmd(0x5, 0x5, 0x5);
	leds.set_dot_correction_cmd_all(0x7F);
	// prepare SPI transmit data as bytes
	leds.process_register();
	// send the data
	leds.toggle_latch(true);
	leds.send_blocking_transmit();
}

void leds_on()
{
	// refresh buffers
	leds.reset();
	// construct the SPI transmit data
	leds.set_greyscale_cmd_all(0x7FFF);
	// prepare SPI transmit data as bytes
	leds.process_register();
	// send the data
	leds.toggle_latch(false);
	leds.send_blocking_transmit();
}

void leds_off()
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
}

void mainapp()
{
	// initial setup may fail during Power-On-Reset (POR) so do it twice
	setup_leds();
	setup_leds();

	static ssd1306::Font5x7 font;
	static ssd1306::Font16x26 font2;
	static ssd1306::Display oled;

	oled.init();
	uint8_t count = 0;

	#ifdef USE_FULL_LL_DRIVER
		// set the max freq (16MHz) for PWM when TIM4_ARR is also 1.
		// LL_TIM_OC_SetCompareCH1(TIM4, 0x1);
		// enable the timer but not the output
		LL_TIM_EnableCounter(TIM4);
	#endif    	

	//uint32_t delay_ms {0};
	while(true)
	{
		
		leds_off();
		
		// run oled animation		
		std::string msg {font2.character_map[count]};
		oled.write(msg, font2, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
		if (count < font2.character_map.size() - 1) { count++; }
		else { count = 0; }		

		#ifdef USE_FULL_LL_DRIVER
			embed_utils::tim::ll_delay_microsecond(TIM14, 0x01);
			// LL_mDelay(100);
		#endif

		leds_on();
		
		#ifdef USE_FULL_LL_DRIVER
			embed_utils::tim::ll_delay_microsecond(TIM14, 0x01);
			// LL_mDelay(100);
		#endif
	}
}



#ifdef __cplusplus
}
#endif
