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
#include <ff.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

void mainapp()
{	
	fatfs::ff fatfs_handle;
	fatfs::FATFS fs;
	fatfs::FIL fil;
	char sd_path[4];          /* uSD device logical drive path */
	fatfs::FRESULT fres;
	fres = fatfs_handle.f_mount(&fs, (fatfs::TCHAR const*)sd_path, 1); //1=mount now
	fres = fatfs_handle.f_open(&fil, (fatfs::TCHAR const*)sd_path, 1);

	std::array<char, 100> read_buff;
	fatfs::UINT bytes_read {0};
	fres = fatfs_handle.f_read(&fil, read_buff.data(), read_buff.size(), &bytes_read);
	
	std::array<char, 100> write_buff;
	fatfs::UINT bytes_written {0};
	fres = fatfs_handle.f_write(&fil, write_buff.data(), write_buff.size(), &bytes_written);

	if (fres != fatfs::FRESULT::FR_OK) {
		// something bad happened
	}	

	// initialise the timer used for system wide microsecond timeout
	stm32::TimerManager::initialise(TIM6);

	// The USART and Timer used to send the MIDI heartbeat
	midi_stm32::DeviceInterface<stm32::isr::InterruptTypeStm32g0> midi_usart_interface(
		USART5,
		stm32::isr::InterruptTypeStm32g0::usart5
	);

	// Timer peripheral for sequencer manager tempo control
	bass_station::tempo_timer_pair_t sequencer_tempo_timer_pair {TIM3, stm32::isr::InterruptTypeStm32g0::tim3};

	// Timer peripheral for sequencer manager rotary encoder control
	TIM_TypeDef *sequencer_encoder_timer = TIM1;

	// SPI peripheral for SSD1306 display driver serial communication
	ssd1306::DriverSerialInterface<stm32::isr::InterruptTypeStm32g0> ssd1306_spi_interface(
		SPI1, 
		GPIOA, 
		LL_GPIO_PIN_0, 
		GPIOA, 
		LL_GPIO_PIN_3,
		stm32::isr::InterruptTypeStm32g0::dma1_ch2);

	// I2C peripheral for keypad manager serial communication
	I2C_TypeDef *ad5587_keypad_i2c = I2C3;

	// keypad debounce timer
	TIM_TypeDef *general_purpose_debounce_timer = TIM17;

	// I2C periperhal for synth output control switch serial communication
	I2C_TypeDef *adg2188_control_sw_i2c = I2C2;

	// SPI peripheral for TLC5955 LED driver serial communication
	tlc5955::DriverSerialInterface tlc5955_spi_interface(
		SPI2, 
		GPIOB, 
		LL_GPIO_PIN_9, 
		GPIOB, 
		LL_GPIO_PIN_7, 
		GPIOB, 
		LL_GPIO_PIN_8,
		TIM4,
		TIM_CCER_CC1E,
		LL_SYSCFG_I2C_FASTMODEPLUS_PB7,
		LL_SYSCFG_I2C_FASTMODEPLUS_PB8,
		LL_IOP_GRP1_PERIPH_GPIOB,
		LL_APB1_GRP1_PERIPH_SPI2);


	// initialise the sequencer
	static bass_station::SequenceManager sequencer(
		sequencer_tempo_timer_pair,
		sequencer_encoder_timer,
		ssd1306_spi_interface, 
		ad5587_keypad_i2c, 
		general_purpose_debounce_timer,
		adg2188_control_sw_i2c, 
		tlc5955_spi_interface,
		midi_usart_interface);

	sequencer.start_loop();
	// we should never get past here	
}

#ifdef __cplusplus
}
#endif
