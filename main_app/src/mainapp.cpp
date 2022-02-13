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
#include <sequence_manager.hpp>
#include <timer_manager.hpp>
#include <adp5587.hpp>


#ifdef __cplusplus
extern "C"
{
#endif

void mainapp()
{	
	// initialise the timer used for system wide microsecond timeout
	stm32::TimerManager::initialise(TIM14);

	// Timer peripheral for sequencer manager tempo control
	TIM_TypeDef *sequencer_tempo_timer = TIM15;

	// Timer peripheral for sequencer manager rotary encoder control
	TIM_TypeDef *sequencer_encoder_timer = TIM1;

	// SPI peripheral for SSD1306 display driver serial communication
	ssd1306::DriverSerialInterface ssd1306_spi_interface(
		SPI1, 
		SPI1_DC_GPIO_Port, 
		SPI1_DC_Pin, 
		SPI1_RESET_GPIO_Port, 
		SPI1_RESET_Pin);

	// Timer peripheral for display manager refresh rate control
	TIM_TypeDef *display_refresh_timer = TIM16;

	// I2C peripheral for keypad manager serial communication
	I2C_TypeDef *ad5587_keypad_i2c = I2C3;

	// keypad debounce timer
	TIM_TypeDef *ad5587_debounce_timer = TIM17;

	// I2C periperhal for synth output control switch serial communication
	I2C_TypeDef *adg2188_control_sw_i2c = I2C2;

	// SPI peripheral for TLC5955 LED driver serial communication
	tlc5955::DriverSerialInterface tlc5955_spi_interface(
		SPI2, 
		TLC5955_SPI2_LAT_GPIO_Port, 
		TLC5955_SPI2_LAT_Pin, 
		TLC5955_SPI2_MOSI_GPIO_Port, 
		TLC5955_SPI2_MOSI_Pin, 
		TLC5955_SPI2_SCK_GPIO_Port, 
		TLC5955_SPI2_SCK_Pin,
		TIM4,
		LL_TIM_CHANNEL_CH1,
		LL_SYSCFG_I2C_FASTMODEPLUS_PB7,
		LL_SYSCFG_I2C_FASTMODEPLUS_PB8,
		LL_IOP_GRP1_PERIPH_GPIOB,
		LL_APB1_GRP1_PERIPH_SPI2);

	[[maybe_unused]] midi_stm32::DeviceInterface midi_usart_interface(
		USART5,
		stm32::isr::STM32G0InterruptManager::InterruptType::usart5,
		TIM7,
		stm32::isr::STM32G0InterruptManager::InterruptType::tim7
	);
	
	std::pair<std::unique_ptr<TIM_TypeDef>, stm32::isr::STM32G0InterruptManager::InterruptType> tempo_timer 
	{
		TIM15,
		stm32::isr::STM32G0InterruptManager::InterruptType::tim15
	};

	// initialise the sequencer
	static bass_station::SequenceManager sequencer(
		sequencer_tempo_timer, 
		sequencer_encoder_timer,
		ssd1306_spi_interface, 
		display_refresh_timer, 
		ad5587_keypad_i2c, 
		ad5587_debounce_timer,
		adg2188_control_sw_i2c, 
		tlc5955_spi_interface,
		midi_usart_interface);

	sequencer.start_loop();
	// we should never get here	
}

#ifdef __cplusplus
}
#endif
