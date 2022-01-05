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
#include <SequencerLedManager.hpp>


#ifdef __cplusplus
extern "C"
{
#endif

uint8_t font_count = 0;
ssd1306::Font16x26 font2;
ssd1306::Display oled;


void update_oled()
{
	std::string msg {font2.character_map[font_count]};
	oled.write(msg, font2, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
	if (font_count < font2.character_map.size() - 1) { font_count++; }
	else { font_count = 0; }	
}

void mainapp()
{
	// setup display driver
	oled.init();

	// setup led driver
	bass_station::SequencerLedManager led_manager;

	// variables used for sequencer LED demo
    uint16_t pwm_value = 0xFFFF;
    led_manager.send_control_data();
    uint16_t delay {25};

	while(true)
	{

		// update sequencer LEDs
		led_manager.update_ladder_demo(pwm_value, delay);

		// update oled animation		
		update_oled();					

	}
}



#ifdef __cplusplus
}
#endif
