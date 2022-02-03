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


KeypadManager::KeypadManager(I2C_TypeDef *i2c_handle) : m_keypad_driver(adp5587::Driver(i2c_handle))
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

    m_keypad_driver.gpio_interrupt_select(
        0x00,
        0x00,
        m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);
    
    m_keypad_driver.gpio_fifo_select(
        0x00,
        0x00,
        m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);

}

void KeypadManager::get_key_events(std::array<adp5587::Driver::KeyPadMappings, 10> &key_events_list)
{
    m_keypad_driver.get_key_events(key_events_list);
}

} // namespace bass_station