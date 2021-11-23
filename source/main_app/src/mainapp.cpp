/*
 * mainapp.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#include "mainapp.hpp"
#include <ssd1306.hpp>
#include <tlc5955.hpp>

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
		xs_font.set_font(fd.Font3x5);

		// ssd1306::Font small_font;
		// small_font.init(fd.Font5x7);

		// ssd1306::Font med_font;
		// med_font.init(fd.Font7x10);

		// ssd1306::Font large_font;
		// large_font.init(fd.Font11x18);

		// ssd1306::Font xl_font;
		// xl_font.init(fd.Font16x26);

		ssd1306::Display oled;
		oled.init();

		// oled.fill(ssd1306::Colour::Black);
		// oled.set_cursor(2, 0);
		// std::stringstream text("Init LEDS");
		// oled.write_string(text, small_font, ssd1306::Colour::White, 3);
		// oled.update_screen();
		
		std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc {127};
		std::bitset<tlc5955::Driver::m_mc_data_resolution> led_mc {4};
		std::bitset<tlc5955::Driver::m_dc_data_resolution> led_dc {127};
		std::bitset<tlc5955::Driver::m_gs_data_resolution> led_gs {32767};
		tlc5955::Driver leds;
		uint16_t bit_test_results = leds.startup_tests();
		UNUSED(bit_test_results);
		// leds.set_control_bit(true);
		// leds.set_ctrl_cmd_bits();
		// leds.set_padding_bits();
		// leds.set_function_data(true, true, true, true, true);

		// leds.set_bc_data(led_bc, led_bc, led_bc);
		// leds.set_mc_data(led_mc, led_mc, led_mc);
		// // leds.set_all_dc_data(led_dc, led_dc, led_dc);
		// leds.send_data();
		//leds.flush_common_register();
		
		//leds.send_control_data();
		uint8_t count = 0;

		while(true)
		{
			std::array<char, 10> digit_ascii {'0','1','2','3','4','5','6','7','8','9'};
			std::stringstream msg;
			msg << digit_ascii[count];
			oled.write(msg, xs_font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
			if (count < digit_ascii.size() - 1) { count++; }
			else { count = 0; }		

			//leds.set_control_bit(false);
			//leds.set_all_gs_data(led_gs, led_gs, led_gs);
	//		leds.send_data();
			//leds.flush_common_register();

			//HAL_Delay(1);
			// leds.flush_common_register();
			//HAL_Delay(1);
			//HAL_GPIO_WritePin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin, GPIO_PIN_RESET);
		}
	}


#ifdef __cplusplus
	}
#endif