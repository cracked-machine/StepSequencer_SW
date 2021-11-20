/*
 * mainapp.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#include "mainapp.hpp"
#include <ssd1306.hpp>

#include <sstream>

#ifdef __cplusplus
	extern "C"
	{
#endif

	void test_border(ssd1306::Display &oled) {
	    oled.fill(ssd1306::Colour::Black);

	    uint32_t start = HAL_GetTick();
	    uint32_t end = start;
	    uint8_t x = 0;
	    uint8_t y = 0;
	    do {
	        oled.draw_pixel(x, y, ssd1306::Colour::Black);

	        if((y == 0) && (x < 127))
	            x++;
	        else if((x == 127) && (y < 63))
	            y++;
	        else if((y == 63) && (x > 0))
	            x--;
	        else
	            y--;

	        oled.draw_pixel(x, y, ssd1306::Colour::White);
	        oled.update_screen();

	        HAL_Delay(5);
	        end = HAL_GetTick();
	    } while((end - start) < 8000);

	    HAL_Delay(1000);
	}

	void mainapp()
	{
		ssd1306::FontData fd;
		ssd1306::Font xs_font;
		xs_font.init(fd.Font3x5);

		ssd1306::Font small_font;
		small_font.init(fd.Font5x7);

		ssd1306::Font med_font;
		med_font.init(fd.Font7x10);

		ssd1306::Font large_font;
		large_font.init(fd.Font11x18);

		ssd1306::Font xl_font;
		xl_font.init(fd.Font16x26);

		ssd1306::Display oled;
		oled.init();


		while(true)
		{
			test_border(oled);

			oled.fill(ssd1306::Colour::Black);
			oled.set_cursor(2, 0);
			std::stringstream text("TEST");
			oled.write_string(text, xl_font, ssd1306::Colour::White, 0);
			oled.update_screen();

			HAL_Delay(1000);
			oled.fill(ssd1306::Colour::Black);
			oled.update_screen();
			HAL_Delay(1000);

		}

	}


#ifdef __cplusplus
	}
#endif
