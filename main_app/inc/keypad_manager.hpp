// MIT License

// Copyright (c) 2022 Chris Sutton

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

#ifndef __KEYPAD_MANAGER_HPP__
#define __KEYPAD_MANAGER_HPP__

#include <adp5587.hpp>
#include <step.hpp>

#if defined(X86_UNIT_TESTING_ONLY)
	// only used when unit testing on x86
	#include <mock_cmsis.hpp>
#else
    #include <timer_manager.hpp>	
#endif



// The ADP5587 HW maps the following values to the keys press/release events on the bass station sequencer
//              1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16
//  UpperRow    131/3   141/13  151/23  161/33  171/43  181/53  191/63  201/73  132/4   142/14  152/24  162/34  172/44  182/54  192/64  202/74
//  LowerRow    129/1   139/11  149/21  159/31  169/41  179/51  189/61  199/71  130/2   140/12  150/22  160/32  170/42  180/52  190/62  200/72


namespace bass_station
{

enum class UserKeyStates
{
    STOPPED,
    RUNNING,
    IDLE,
};


/// @brief This is really just a wrapper for the ADP5587 driver at the moment
/// @todo Some of the SequenceManager functionality should be moved into here at some point...
class KeypadManager
{
public:
    /// @brief Construct a new Keypad Manager object
    /// @param i2c_handle The ADP5587 I2C interface
    /// @param debounce_timer The debouce timer for the keypad
    KeypadManager(I2C_TypeDef *i2c_handle, TIM_TypeDef *debounce_timer);

    /// @brief Get the key events object
    /// @param key_events_list 
    void get_key_events(std::array<adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings, 10> &key_events_list);
    
    /// @brief Update step_sequence map param with latest Keypad events and return the latest UserKey press.
    // @param step_sequence The map object containing current pattern data
    // @return UserKeyStates Latest UserKey press
    UserKeyStates process_key_events(
        noarch::containers::StaticMap<
            adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings, 
            bass_station::Step, 
            32U
        > &step_sequence);

    // store the index of the last key selected by the user. We can use this index to lookup the position in the StaticMap later on.
    uint8_t last_user_selected_key_idx{0};

private:
    // @brief The ADP5587 keypad driver 
    adp5587::Driver<stm32::isr::InterruptTypeStm32g0> m_keypad_driver;

    /// @brief The timer for pattern key debounce
    std::unique_ptr<TIM_TypeDef> m_debounce_timer;

    /// @brief The allowable delay between pressing keys on the sequence keypad
    /// Increasing this value will decrease bounce but also responsiveness
    const uint32_t m_pattern_debounce_threshold_ms{350};    
    /// @brief Store the last timer count for debounce
    uint32_t m_last_pattern_debounce_count_ms{0};


    static constexpr uint8_t StartButtonID = static_cast<uint8_t>(adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::GPIKeyMappings::C8 | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::GPIKeyMappings::ON);
    static constexpr uint8_t StopButtonID = static_cast<uint8_t>(adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::GPIKeyMappings::C7 | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::GPIKeyMappings::ON);
};

} // namespace bass_station

#endif // __KEYPAD_MANAGER_HPP__