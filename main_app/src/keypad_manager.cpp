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


#include <keypad_manager.hpp>

namespace bass_station
{

KeypadManager::KeypadManager(I2C_TypeDef *i2c_handle, TIM_TypeDef *debounce_timer) 
: m_keypad_driver(adp5587::Driver(i2c_handle)), m_debounce_timer(debounce_timer)
{
    
    // 1) Enable keypad interrupts
    m_keypad_driver.enable_keypad_isr();
    m_keypad_driver.enable_gpio_isr();

    // 2) Enable the keypad rows and columns 
    m_keypad_driver.keypad_gpio_select(
        (m_keypad_driver.KP_GPIO::R0 | m_keypad_driver.KP_GPIO::R1 | m_keypad_driver.KP_GPIO::R2 | m_keypad_driver.KP_GPIO::R3 | 
        m_keypad_driver.KP_GPIO::R4 | m_keypad_driver.KP_GPIO::R5 | m_keypad_driver.KP_GPIO::R6 | m_keypad_driver.KP_GPIO::R7), 
        (m_keypad_driver.KP_GPIO::C0 | m_keypad_driver.KP_GPIO::C1 | m_keypad_driver.KP_GPIO::C2 | m_keypad_driver.KP_GPIO::C3),
        0x00);

    // 3) Select the two columns for GPIO interrupts
    m_keypad_driver.gpio_interrupt_select(
        0x00,
        0x00,
        m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);
    
    // 4) Select the two columns for inclusion in the key event FIFO registers
    m_keypad_driver.gpio_fifo_select(
        0x00,
        0x00,
        m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);

    // start the debounce timer for the keypad
    LL_TIM_EnableCounter(m_debounce_timer.get());        

}

void KeypadManager::process_key_events(
    noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, bass_station::Step, 32U> &sequence_map)
{
    // get the key events FIFO list from the ADP5587 driver 
    std::array<adp5587::Driver::KeyPadMappings, 10U> key_events_list;
    get_key_events(key_events_list);
    
    // process each key event in turn (if any)
    for (adp5587::Driver::KeyPadMappings key_event : key_events_list)
    {
        // find the key event that matches the sequence step
        Step *step = sequence_map.find_key(key_event);
        if(step == nullptr) { /* no match found in map */ }
        else
        {
            // only toggle key state if debounce conditions are met
            [[maybe_unused]] uint32_t timer_count_ms = LL_TIM_GetCounter(m_debounce_timer.get());
            if ((timer_count_ms - m_last_debounce_count_ms > m_debounce_threshold_ms) && (timer_count_ms > m_last_debounce_count_ms))
            {
                // flip the key state
                step->m_key_state = static_cast<KeyState>( !(step->m_key_state) );
            }
            m_last_debounce_count_ms = timer_count_ms;
        }
    }
}

void KeypadManager::get_key_events(std::array<adp5587::Driver::KeyPadMappings, 10> &key_events_list)
{
    m_keypad_driver.get_key_events(key_events_list);
}

void KeypadManager::translate_sw_pole_to_note_string(NoteSwitchMapping sw, std::string &note)
{
    switch(sw)
    {
        case bass_station::NoteSwitchMapping::c0:
            note = "C0 ";
            break;
        case bass_station::NoteSwitchMapping::c0_sharp:
            note = "C0#";
            break;
        case bass_station::NoteSwitchMapping::d0:
            note = "D0 ";
            break;
        case bass_station::NoteSwitchMapping::d0_sharp:
            note = "D0#";
            break;
        case bass_station::NoteSwitchMapping::e0:
            note = "E0 ";
            break;
        case bass_station::NoteSwitchMapping::f0:
            note = "F0 ";
            break;
        case bass_station::NoteSwitchMapping::f0_sharp:
            note = "F0#";
            break;
        case bass_station::NoteSwitchMapping::g0:
            note = "G0 ";
            break;
        case bass_station::NoteSwitchMapping::g0_sharp:
            note = "G0#";
            break;
        case bass_station::NoteSwitchMapping::a1:
            note = "A1 ";
            break;
        case bass_station::NoteSwitchMapping::a1_sharp:
            note = "A1#";
            break;
        case bass_station::NoteSwitchMapping::b1:
            note = "B1 ";
            break;
        case bass_station::NoteSwitchMapping::c1:
            note = "C1 ";
            break;
        case bass_station::NoteSwitchMapping::c1_sharp:
            note = "C1#";
            break;
        case bass_station::NoteSwitchMapping::d1:
            note = "D1 ";
            break;
        case bass_station::NoteSwitchMapping::d1_sharp:
            note = "D1#";
            break;
        case bass_station::NoteSwitchMapping::e1:
            note = "E1 ";
            break;
        case bass_station::NoteSwitchMapping::f1:
            note = "F1 ";
            break;
        case bass_station::NoteSwitchMapping::f1_sharp:
            note = "F1#";
            break;
        case bass_station::NoteSwitchMapping::g1:
            note = "G1 ";
            break;
        case bass_station::NoteSwitchMapping::g1_sharp:
            note = "G1#";
            break;
        case bass_station::NoteSwitchMapping::a2:
            note = "A2 ";
            break;
        case bass_station::NoteSwitchMapping::a2_sharp:
            note = "A2#";
            break;
        case bass_station::NoteSwitchMapping::b2:
            note = "B2 ";
            break;
        case bass_station::NoteSwitchMapping::c2:
            note = "C2 ";
            break;        
        case bass_station::NoteSwitchMapping::none:
            note = "---";
            break;
    }
}

} // namespace bass_station