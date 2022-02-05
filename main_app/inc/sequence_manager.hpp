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

#ifndef __SEQUENCE_MANAGER_HPP__
#define __SEQUENCE_MANAGER_HPP__

#include <array>

#include <led_manager.hpp>
#include <keypad_manager.hpp>
#include <static_map.hpp>
#include <display_manager.hpp>

namespace bass_station
{

// This class takes user key input and controls key LEDs (via LEDManager) and output synth control (via adg2188)
class SequenceManager
{
public:

    /// @brief Construct a new Sequence Manager
    /// @param sequencer_tempo_timer The SequenceManager timer for looping the sequence pattern
    /// @param sequencer_encoder_timer The SequenceManager rotary encoder interface
    /// @param display_spi The DisplayManager SPI interface
    /// @param display_refresh_timer The DisplayManager refresh rate timer
    /// @param ad5587_keypad_i2c The KeypadManager I2C interface        
    /// @param ad5587_keypad_debounce_timer  The KeypadManager debouce timer
    /// @param adg2188_control_sw_i2c The crosspoint switch I2C interface for controlling the synth notes
    /// @param tlc5955_led_spi The LedManager SPI interface
    SequenceManager(
        TIM_TypeDef *sequencer_tempo_timer, 
        TIM_TypeDef *sequencer_encoder_timer,
        ssd1306::DriverSerialInterface display_spi, 
        TIM_TypeDef *display_refresh_timer,
        I2C_TypeDef *ad5587_keypad_i2c,
        TIM_TypeDef *ad5587_keypad_debounce_timer,
        I2C_TypeDef *adg2188_control_sw_i2c,
        SPI_TypeDef *tlc5955_led_spi);

private:

    NoteSwitchMapping m_previous_enabled_note{NoteSwitchMapping::e0};

    // The default sequence data
    static std::array< std::pair< adp5587::Driver::KeyPadMappings, Step >, 32 > sequence_data;

    /// @brief Map holding the 32-step sequence pattern (Sequencer::sequence_data)
    noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, sequence_data.size()> m_sequence_map = 
        noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, sequence_data.size()>{{sequence_data}};

    /// @brief The timer for tempo of the sequencer
    std::unique_ptr<TIM_TypeDef> m_sequencer_tempo_timer;

    /// @brief The timer used for rotary encoder
    std::unique_ptr<TIM_TypeDef> m_sequencer_encoder_timer;

    /// @brief Manages the SSD1306 OLED display
    bass_station::DisplayManager m_ssd1306_display_spi;

    /// @brief Manages the ADP5587 keyscanner/io expander chip
    bass_station::KeypadManager m_ad5587_keypad_i2c;

    /// @brief Manages the ADG2188 crosspoint switch chip
    adg2188::Driver m_synth_control_switch;

    /// @brief Manages the TLC5955 chip
    bass_station::LedManager m_led_manager;

    /// @brief counter for sequencer position, incremented in increment_and_execute_sequence_step()
    uint8_t m_step_position {0};

    /// @brief This determines the positional order in which the cursor sweeps the sequence
    // This begins on the upper row and ends on the lower row, sweeping left to right
    std::array<uint8_t, 32> m_sequencer_key_mapping {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    /// @brief Registers ISR callback with STM32G0InterruptManage
    struct TempoTimerIntHandler : public stm32::isr::STM32G0InterruptManager
	{
        /// @brief the parent driver class
        SequenceManager *m_seq_man_ptr;

		/// @brief initialise and register this handler instance with STM32G0InterruptManager
		// @param parent_driver_ptr the instance to register
		void initialise(SequenceManager *seq_man_ptr)
		{
			m_seq_man_ptr = seq_man_ptr;
			// register pointer to this handler class in stm32::isr::STM32G0InterruptManager
			stm32::isr::STM32G0InterruptManager::register_handler(stm32::isr::STM32G0InterruptManager::InterruptType::tim16, this);
		}        
        /// @brief The callback used by STM32G0InterruptManager
		virtual void ISR()
		{
            m_seq_man_ptr->tempo_timer_isr();
		}        
	};
	/// @brief TempoTimerIntHandler member
    TempoTimerIntHandler m_sequencer_tempo_timer_isr_handler;

    /// @brief SequenceManager callback, the main sequencer execution loop
    void tempo_timer_isr();

    /// @brief Update the display and tempo timer
    void update_display_and_tempo();

    /// @brief Runs the note/step sequence
    /// @param run_demo_only 
    void increment_and_execute_sequence_step(bool run_demo_only = false);



};


}   // namespace bass_station


#endif // __SEQUENCE_MANAGER_HPP__