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


// @file mainapp.cpp
// @author Chris Sutton (chrisjsutton@gmail.com)
// @brief 
// @version 0.1
// @date 2022-01-07


#include "mainapp.hpp"

#include <ssd1306.hpp>
#include <SequencerLedManager.hpp>
#include <adp5587.hpp>
#include <adg2188.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

uint8_t font_count = 0;
ssd1306::Font5x7 font2;
ssd1306::Display oled;


void update_oled(std::string &msg)
{
	std::string font_char {font2.character_map[font_count]};
	std::string final_msg = msg + font_char;
	ssd1306::ErrorStatus res = oled.write(final_msg, font2, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
	if (res != ssd1306::ErrorStatus::OK) 
	{ 
		#if defined(USE_RTT) 
			SEGGER_RTT_printf(0, "ssd1306::Display::write(): Error\n"); 
		#endif	
	}

	if (font_count < font2.character_map.size() - 1) { font_count++; }
	else { font_count = 0; }	
}


bool check_i2c_addr(uint8_t addr)
{

	LL_I2C_TransmitData8(I2C3, addr << 1);
	if ( (LL_I2C_IsActiveFlag_NACK(I2C3)) || (LL_I2C_IsActiveFlag_STOP(I2C3)) )
	{
		return true;
	}
	return false;
}

// setup TLC5955 IC LED driver
bass_station::SequencerLedManager led_manager;
uint16_t pwm_value = 0xFFFF;

void mainapp()
{
	// setup SSD1306 IC display driver
	oled.init();

	// setup ADP5587 IC keyscan driver
	adp5587::Driver keyscanner [[maybe_unused]];
	adg2188::Driver xpoint [[maybe_unused]];



	// variables used for sequencer LED demo

    led_manager.send_control_data();
    uint16_t delay {10};

	while(true)
	{

		// update oled animation
		std::string msg {"Hello "};
		update_oled(msg);	

		// update sequencer LEDs
		// led_manager.update_ladder_demo(pwm_value, delay);
		LL_mDelay(delay);
		led_manager.clear_all_leds();

		keyscanner.process_fifo();
				

	}
}


void EXTI4_15_IRQHandler(void)
{

	if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_5) != RESET)
	{
		led_manager.send_greyscale_data(
			0, 
			bass_station::SequencerLedManager::SequencerRow::lower, 
			pwm_value, 
			bass_station::SequencerLedManager::LedColour::magenta,
			bass_station::SequencerLedManager::LatchOption::enable);

		#if defined(USE_RTT) 
			SEGGER_RTT_printf(0, "\nEXTI5 ISR\n");
		#endif
		LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_5);
	}
}

#ifdef __cplusplus
}
#endif
