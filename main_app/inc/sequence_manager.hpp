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
#include <step.hpp>
#include <led_manager.hpp>
#include <keypad_manager.hpp>
#include <static_map.hpp>
#include <adg2188.hpp>
#include <display_manager.hpp>

// The ADP5587 HW maps the following values to the keys press/release events on the bass station sequencer
//              1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16
//  UpperRow    131/3   141/13  151/23  161/33  171/43  181/53  191/63  201/73  132/4   142/14  152/24  162/34  172/44  182/54  192/64  202/74
//  LowerRow    129/1   139/11  149/21  159/31  169/41  179/51  189/61  199/71  130/2   140/12  150/22  160/32  170/42  180/52  190/62  200/72

namespace bass_station
{




constexpr LedColour m_default_colour { LedColour::white };
constexpr LedColour m_beat_colour_off { LedColour::white };
constexpr LedColour m_beat_colour_on { LedColour::blue };

// The sequence data
static std::array< std::pair< adp5587::Driver::KeyPadMappings, Step >, 32 > key_data{
{


    {adp5587::Driver::KeyPadMappings::A0_ON, Step(KeyState::ON,  m_default_colour, 0,   4,  16)},
    {adp5587::Driver::KeyPadMappings::A1_ON, Step(KeyState::OFF, m_default_colour, 1,   0,  17)},
    {adp5587::Driver::KeyPadMappings::A2_ON, Step(KeyState::OFF, m_default_colour, 2,   5,  18)},
    {adp5587::Driver::KeyPadMappings::A3_ON, Step(KeyState::OFF, m_default_colour, 3,   1,  19)},
    {adp5587::Driver::KeyPadMappings::A4_ON, Step(KeyState::ON,  m_default_colour, 4,   2,  20)},
    {adp5587::Driver::KeyPadMappings::A5_ON, Step(KeyState::OFF, m_default_colour, 5,   6,  21)},
    {adp5587::Driver::KeyPadMappings::A6_ON, Step(KeyState::OFF, m_default_colour, 6,   3,  22)},
    {adp5587::Driver::KeyPadMappings::A7_ON, Step(KeyState::OFF, m_default_colour, 7,   7,  23)},

    {adp5587::Driver::KeyPadMappings::B0_ON, Step(KeyState::ON,  m_default_colour, 8,   11, 24)},
    {adp5587::Driver::KeyPadMappings::B1_ON, Step(KeyState::OFF, m_default_colour, 9,   15, 25)},
    {adp5587::Driver::KeyPadMappings::B2_ON, Step(KeyState::OFF, m_default_colour, 10,  10, 26)},
    {adp5587::Driver::KeyPadMappings::B3_ON, Step(KeyState::OFF, m_default_colour, 11,  14, 27)},
    {adp5587::Driver::KeyPadMappings::B4_ON, Step(KeyState::ON,  m_default_colour, 12,  13, 28)},
    {adp5587::Driver::KeyPadMappings::B5_ON, Step(KeyState::OFF, m_default_colour, 13,  9,  29)},
    {adp5587::Driver::KeyPadMappings::B6_ON, Step(KeyState::OFF, m_default_colour, 14,  12, 30)},
    {adp5587::Driver::KeyPadMappings::B7_ON, Step(KeyState::OFF, m_default_colour, 15,  8,  31)}, 

    {adp5587::Driver::KeyPadMappings::C0_ON, Step(KeyState::ON,  m_default_colour, 0,   7,  0)},
    {adp5587::Driver::KeyPadMappings::C1_ON, Step(KeyState::OFF, m_default_colour, 1,   3,  1)},
    {adp5587::Driver::KeyPadMappings::C2_ON, Step(KeyState::OFF, m_default_colour, 2,   6,  2)},
    {adp5587::Driver::KeyPadMappings::C3_ON, Step(KeyState::OFF, m_default_colour, 3,   2,  3)},
    {adp5587::Driver::KeyPadMappings::C4_ON, Step(KeyState::ON,  m_default_colour, 4,   1,  4)},
    {adp5587::Driver::KeyPadMappings::C5_ON, Step(KeyState::OFF, m_default_colour, 5,   5,  5)},
    {adp5587::Driver::KeyPadMappings::C6_ON, Step(KeyState::OFF, m_default_colour, 6,   0,  6)},
    {adp5587::Driver::KeyPadMappings::C7_ON, Step(KeyState::OFF, m_default_colour, 7,   4,  7)},  

    {adp5587::Driver::KeyPadMappings::D0_ON, Step(KeyState::ON,  m_default_colour, 8,   8,  8)},
    {adp5587::Driver::KeyPadMappings::D1_ON, Step(KeyState::OFF, m_default_colour, 9,   12, 9)},
    {adp5587::Driver::KeyPadMappings::D2_ON, Step(KeyState::OFF, m_default_colour, 10,  9,  10)},
    {adp5587::Driver::KeyPadMappings::D3_ON, Step(KeyState::OFF, m_default_colour, 11,  13, 11)},
    {adp5587::Driver::KeyPadMappings::D4_ON, Step(KeyState::ON,  m_default_colour, 12,  14, 12)},
    {adp5587::Driver::KeyPadMappings::D5_ON, Step(KeyState::OFF, m_default_colour, 13,  10, 13)},
    {adp5587::Driver::KeyPadMappings::D6_ON, Step(KeyState::OFF, m_default_colour, 14,  15, 14)},
    {adp5587::Driver::KeyPadMappings::D7_ON, Step(KeyState::OFF, m_default_colour, 15,  11, 15)},       
    
}};

// This class takes user key input and controls key LEDs (via LEDManager) and output synth control (via adg2188)
class SequenceManager
{
public:
    /// @brief Construct a new Sequence Manager object
    // @param timer Used for sequence run tempo
    SequenceManager(
        TIM_TypeDef *sequencer_tempo_timer, 
        TIM_TypeDef *sequencer_encoder_timer,
        SPI_TypeDef *display_spi, 
        TIM_TypeDef *display_refresh_timer,
        I2C_TypeDef *ad5587_keypad_i2c,
        I2C_TypeDef *adg2188_control_sw_i2c,
        SPI_TypeDef *tlc5955_led_spi,
        TIM_TypeDef *debounce_timer);
    
private:

    /// @brief The timer for tempo of the sequencer
    std::unique_ptr<TIM_TypeDef> m_sequencer_tempo_timer;
    /// @brief The timer used for rotary encoder
    std::unique_ptr<TIM_TypeDef> m_sequencer_encoder_timer;

    /// @brief Manages the SSD1306 OLED display
    bass_station::DisplayManager m_ssd1306_display_spi;
    /// @brief Manages the ADP5587 chip
    bass_station::KeypadManager m_ad5587_keypad_i2c;
    /// @brief Manages the ADG2188 crosspoint switch chip
    adg2188::Driver m_synth_control_switch;
    /// @brief Manages the TLC5955 chip
    bass_station::LedManager m_led_manager;
    /// @brief The timer used to test m_debounce_threshold
    std::unique_ptr<TIM_TypeDef> m_debounce_timer;

    /// @brief counter for sequencer position, incremented in increment_and_execute_sequence_step()
    uint8_t m_step_position {0};

    /// @brief This determines the positional order in which the cursor sweeps the sequence
    // This begins on the upper row and ends on the lower row, sweeping left to right
    std::array<uint8_t, 32> m_sequencer_key_mapping {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    /// @brief Map holding the 32-step sequence data
    noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, key_data.size()> m_sequence_map = 
        noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, key_data.size()>{{key_data}};

    /// @brief Registers ISR callback with STM32G0InterruptManager
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

    /// @brief poll the ADP5587 keyscanner for the latest key event data
    /// and update the m_sequence map
    void process_key_events();


    // @brief Store the last timer count for debounce
    uint32_t m_last_debounce_count_ms{0};
    // @brief Requirements fo debounce
    uint32_t m_debounce_threshold_ms{500};
};


}   // namespace bass_station


#endif // __SEQUENCE_MANAGER_HPP__