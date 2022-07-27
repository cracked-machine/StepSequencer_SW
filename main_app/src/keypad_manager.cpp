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

#include <keypad_manager.hpp>

namespace bass_station
{

KeypadManager::KeypadManager(I2C_TypeDef *i2c_handle, TIM_TypeDef *debounce_timer)
    : m_keypad_driver(adp5587::Driver<STM32G0_ISR>(i2c_handle)),
      m_debounce_timer(*debounce_timer)
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

  // 3) Enable interrupts on GPIO inputs
  m_keypad_driver.gpio_interrupt_select(0x00, 0x00, m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);

  // 4) set GPIO inputs for inclusion in the key event FIFO registers
  m_keypad_driver.gpio_fifo_select(0x00,
                                   m_keypad_driver.KP_GPIO::C4 | m_keypad_driver.KP_GPIO::C5 | m_keypad_driver.KP_GPIO::C6 |
                                       m_keypad_driver.KP_GPIO::C7,
                                   m_keypad_driver.KP_GPIO::C8 | m_keypad_driver.KP_GPIO::C9);

  // start the debounce timer for the keypad
#if not defined(X86_UNIT_TESTING_ONLY)
  m_debounce_timer.CR1 = m_debounce_timer.CR1 | TIM_CR1_CEN;
  // LL_TIM_EnableCounter(m_debounce_timer);
#endif
}

UserKeyStates KeypadManager::process_key_events(noarch::containers::StaticMap<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, Step, 32U> &sequence_map)
{
  UserKeyStates running_status{UserKeyStates::IDLE};

  // get the key events FIFO list from the ADP5587 driver
  std::array<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, 10U> key_events_list;
  get_key_events(key_events_list);

  // process each key event in turn (if any)
  for (adp5587::Driver<STM32G0_ISR>::KeyPadMappings key_event : key_events_list)
  {

    // strict debounce control on the pattern step button presses.
    // if threshold is too short the button will toggle states before user releases the button (annoying)
    // if threshold is too long the button will not be responsive enough.
    uint32_t timer_count_ms = m_debounce_timer.CNT;
    if (timer_count_ms - m_last_pattern_debounce_count_ms > m_pattern_debounce_threshold_ms)
    {

      // update the running status of the overall sequencer if start/stop buttons pressed
      if (static_cast<int>(key_event) == StopButtonID)
      {
        running_status = UserKeyStates::STOPPED;
      }
      if (static_cast<int>(key_event) == StartButtonID)
      {
        running_status = UserKeyStates::RUNNING;
      }

      // find the key event that matches the sequence step
      Step *step = sequence_map.find_key(key_event);
      if (step == nullptr)
      { /* no match found in map */
      }
      else
      {
#if not defined(X86_UNIT_TESTING_ONLY)

        if (step->m_key_state == KeyState::ON)
        {
          if (step->m_colour == default_colour)
          {
            // the key was ON but not highlighted, user selected it so lets highlight it
            step->m_colour = user_select_colour;
          }
          else
          {
            // the key was ON and already highlighted, user selected it so lets switch it off completely
            step->m_colour    = default_colour;
            step->m_key_state = KeyState::OFF;
          }
        }
        else
        {
          // the key was OFF, user selected it so lets highlight it and switch it on!
          step->m_colour    = user_select_colour;
          step->m_key_state = KeyState::ON;
        }

        // de-highlight the previously highlighted key...unless we just selected the same key again, then skip
        if (last_user_selected_key_idx != step->m_sequence_abs_pos_index)
        {
          /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
          sequence_map.data[last_user_selected_key_idx].second.m_colour = default_colour;
        }

#endif

        // store the index position of the user selected step for next key interrupt
        last_user_selected_key_idx = step->m_sequence_abs_pos_index;
      }
      m_last_pattern_debounce_count_ms = timer_count_ms;
    }
  }

  return running_status;
}

void KeypadManager::get_key_events(std::array<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, 10> &key_events_list)
{
  m_keypad_driver.get_key_events(key_events_list);
}

} // namespace bass_station