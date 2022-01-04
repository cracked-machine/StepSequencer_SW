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

// @brief Temp helper function to set the up leds object
void setup_leds()
{
	// refresh buffers
	leds.reset();
	
	// construct the SPI transmit data
	leds.set_ctrl_cmd();
	leds.set_padding_bits();
	leds.set_function_cmd(	
		tlc5955::Driver::DisplayFunction::display_repeat_off,
		tlc5955::Driver::TimingFunction::timing_reset_off,
		tlc5955::Driver::RefreshFunction::auto_refresh_off,
		tlc5955::Driver::PwmFunction::normal_pwm,
		tlc5955::Driver::ShortDetectFunction::threshold_90_percent
	);

	// leds.set_function_cmd(true, false, false, false, true);
	leds.set_global_brightness_cmd(0x1, 0x1, 0x1);
	leds.set_max_current_cmd(0x1, 0x1, 0x1);
	leds.set_dot_correction_cmd_all(0x7F);
	
	// prepare SPI transmit data as bytes
	leds.process_register();

	// send data for top row (no latch)
	leds.send_first_bit(tlc5955::DataLatchType::control);
	leds.send_spi_bytes(tlc5955::LatchPinOption::no_latch);
	
	// send data for bottom row
	leds.send_first_bit(tlc5955::DataLatchType::control);
	leds.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);
}

// @brief Temp helper function to turn on LEDs
// @param pwm brightness value
// @param led_position position: 0-15
void leds_on(uint16_t pwm, uint16_t led_position)
{
	// refresh buffers
	leds.reset();
	
	// set the colour - purple
	leds.set_greyscale_cmd_rgb_at_position(led_position, pwm, 0, pwm);
	
	// prepare SPI transmit data as bytes
	leds.process_register();
	
	// send data for top row (no latch)
	leds.send_first_bit(tlc5955::DataLatchType::greyscale);
	leds.send_spi_bytes(tlc5955::LatchPinOption::no_latch);

	// send data for bottom row
	leds.send_first_bit(tlc5955::DataLatchType::greyscale);
	leds.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);	
}

// @brief Temp helper function to turn of LEDs
void leds_off()
{
	// refresh buffers
	leds.reset();
	leds.process_register();

	// send data for top row (no latch)
	leds.send_first_bit(tlc5955::DataLatchType::greyscale);
	leds.send_spi_bytes(tlc5955::LatchPinOption::no_latch);

	// send data for bottom row	
	leds.send_first_bit(tlc5955::DataLatchType::greyscale);
	leds.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);	

}

void mainapp()
{
	// setup led driver
	uint16_t pwm_counter = 0xFFFF;
	uint16_t led_position = 0;
	setup_leds();

	// setup display driver
	uint8_t font_count = 0;
	static ssd1306::Font5x7 font;
	static ssd1306::Font16x26 font2;
	static ssd1306::Display oled;
	oled.init();

	while(true)
	{
		
		leds_on(pwm_counter, led_position);		
		(led_position >= 15) ? led_position = 0 : led_position++;
		
		// run oled animation		
		std::string msg {font2.character_map[font_count]};
		oled.write(msg, font2, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
		if (font_count < font2.character_map.size() - 1) { font_count++; }
		else { font_count = 0; }		

		#ifdef USE_FULL_LL_DRIVER
			//embed_utils::tim::ll_delay_microsecond(TIM14, 0x1);
			LL_mDelay(200);
		#endif

		leds_off();

		
		#ifdef USE_FULL_LL_DRIVER
			// embed_utils::tim::ll_delay_microsecond(TIM14, 0xFFFE);
			LL_mDelay(200);
		#endif
	}
}



#ifdef __cplusplus
}
#endif
