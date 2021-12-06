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
	
	static ssd1306::Font5x7 font;
	static ssd1306::Display oled;
	oled.init();

	// oled.fill(ssd1306::Colour::Black);
	// oled.set_cursor(2, 0);
	// std::stringstream text("Init LEDS");
	// oled.write_string(text, small_font, ssd1306::Colour::White, 3);
	// oled.update_screen();
	
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc {0b000'0001};	
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc_on {0b111'1111};	
	std::bitset<tlc5955::Driver::m_bc_data_resolution> led_bc_off {0b000'0000};	
	std::bitset<tlc5955::Driver::m_mc_data_resolution> led_mc {0b001};		
	std::bitset<tlc5955::Driver::m_dc_data_resolution> led_dc {0b111'1111};		
	std::bitset<tlc5955::Driver::m_gs_data_resolution> led_gs {32767};

	tlc5955::Driver leds;

	
	leds.flush_common_register();
	leds.set_control_bit(true);
	leds.set_ctrl_cmd_bits();
	leds.set_padding_bits();
	leds.set_function_data(true, false, true, true, true);

	leds.set_bc_data(led_bc, led_bc, led_bc);
	leds.set_mc_data(led_mc, led_mc, led_mc);
	leds.set_all_dc_data(led_dc, led_dc, led_dc);


	// enable the gsclk
	#ifdef USE_HAL_DRIVER
    HAL_GPIO_WritePin(TLC5955_SPI2_GSCLK_GPIO_Port, TLC5955_SPI2_GSCLK_Pin, GPIO_PIN_SET);
	leds.start_dma_transmit();
	#endif

	// std::array<uint8_t, 63> data = {
	// 	0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00,
	// 	0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 
	// 	0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00,
	// 	0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB, 0x00, 0xCB 		
	// };
	// HAL_SPI_Transmit_DMA(&hspi2, data.data(), data.size());

	uint8_t count = 0;
	//uint32_t delay_ms {0};
	while(true)
	{

		std::stringstream msg;
		msg << font.character_map[count];
		oled.write(msg, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true);
		if (count < font.character_map.size() - 1) { count++; }
		else { count = 0; }		

		leds.flush_common_register();
		leds.set_control_bit(false);
		leds.set_all_gs_data(led_gs, led_gs, led_gs);
		// leds.set_control_bit(false);
		// leds.set_all_gs_data(led_gs, led_gs, led_gs);
		
		// SPI blocking mode
		//leds.send_data();

		// SPI DMA mode
// 		#ifdef USE_HAL_DRIVER
// 			HAL_Delay(delay_ms);
// 			if (tlc5955_callback)
// 			{
// 				// HAL_SPI_DMAPause(&hspi2);
// 				// tlc5955_callback = false;
// 				// HAL_SPI_DMAResume(&hspi2);
// //				leds.pause_dma_transmit(true);
// 				leds.toggle_latch();
// 				tlc5955_callback = false;
// 				HAL_SPI_DMAResume(&hspi2);
// //				leds.pause_dma_transmit(false);
// 			}
// 		#else
// 			std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
// 		#endif

		// leds.flush_common_register();
		//HAL_Delay(1);
		//HAL_GPIO_WritePin(TLC5955_SPI2_LAT_GPIO_Port, TLC5955_SPI2_LAT_Pin, GPIO_PIN_RESET);
	}
}



#ifdef __cplusplus
}
#endif
