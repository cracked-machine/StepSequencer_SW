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

#ifndef __KEYPAD_MANAGER_HPP__
#define __KEYPAD_MANAGER_HPP__

#include <adp5587.hpp>
#include <adg2188.hpp>
#include <step.hpp>

// The ADP5587 HW maps the following values to the keys press/release events on the bass station sequencer
//              1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16
//  UpperRow    131/3   141/13  151/23  161/33  171/43  181/53  191/63  201/73  132/4   142/14  152/24  162/34  172/44  182/54  192/64  202/74
//  LowerRow    129/1   139/11  149/21  159/31  169/41  179/51  189/61  199/71  130/2   140/12  150/22  160/32  170/42  180/52  190/62  200/72

namespace bass_station
{

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
    void get_key_events(std::array<adp5587::Driver::KeyPadMappings, 10> &key_events_list);
    
    /// @brief poll the ADP5587 keyscanner for the latest key event data
    /// and update the m_sequence map
    void process_key_events(
        noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, bass_station::Step, 32U> &step_sequence);

private:
    // @brief The ADP5587 keypad driver 
    adp5587::Driver m_keypad_driver;

    /// @brief The timer used to test m_debounce_threshold
    std::unique_ptr<TIM_TypeDef> m_debounce_timer;

    /// @brief Store the last timer count for debounce
    uint32_t m_last_debounce_count_ms{0};
    /// @brief Requirements fo debounce
    uint32_t m_debounce_threshold_ms{500};    
};

} // namespace bass_station

#endif // __KEYPAD_MANAGER_HPP__