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

		

	void mainapp()
	{
		
		static ssd1306::Font5x7 font;
		static ssd1306::Display oled;
		oled.init();

		// oled.fill(ssd1306::Colour::Black);
		// oled.set_cursor(2, 0);
		// std::stringstream text("Init LEDS");
		// oled.write_string(text, small_font, ssd1306::Colour::White, 3);
		// oled.update_screen();
		
		// std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc {127};
		// std::bitset<tlc5955::Driver::m_mc_data_resolution> led_mc {4};
		// std::bitset<tlc5955::Driver::m_dc_data_resolution> led_dc {127};
		// std::bitset<tlc5955::Driver::m_gs_data_resolution> led_gs {32767};
		// tlc5955::Driver leds;
		
		// leds.startup_tests();
		
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
	
			std::stringstream msg;
			msg << font.character_map[count];
			oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
			if (count < font.character_map.size() - 1) { count++; }
			else { count = 0; }		

			//leds.set_control_bit(false);
			//leds.set_all_gs_data(led_gs, led_gs, led_gs);
	//		leds.send_data();
			//leds.flush_common_register();
#ifdef USE_HAL_DRIVER
			HAL_Delay(1000);
#else
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
			// leds.flush_common_register();
			//HAL_Delay(1);
			//HAL_GPIO_WritePin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin, GPIO_PIN_RESET);
		}
	}


#ifdef __cplusplus
	}
#endif
