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

#include <limits>
#include <sequence_manager.hpp>
#include <timer_manager.hpp>
#include <tlc5955.hpp>

/// @brief Cycle sequencer LEDs through primary/secondary colours. Warning, this will replace normal sequencer function.
#define LED_TEST 0
/// @brief Automatically start the sequencer on startup. No user input required.
#define SEQUENCER_AUTOSTART_ON_BOOT 0

namespace bass_station
{

SequenceManager::SequenceManager(tempo_timer_pair_t tempo_timer_pair,
                                 TIM_TypeDef *sequencer_encoder_timer,
                                 ssd1306::DriverSerialInterface<STM32G0_ISR> &display_spi_interface,
                                 I2C_TypeDef *ad5587_keypad_i2c,
                                 TIM_TypeDef *debounce_timer,
                                 I2C_TypeDef *adg2188_control_sw_i2c,
                                 tlc5955::DriverSerialInterface &led_spi_interface,
                                 midi_stm32::DeviceInterface<STM32G0_ISR> &midi_usart_interface)

    : m_tempo_timer_device(*tempo_timer_pair.first),
      m_tempo_timer_isr(tempo_timer_pair.second),
      m_sequencer_encoder_timer(*sequencer_encoder_timer),
      m_ssd1306_display_spi(bass_station::DisplayManager(display_spi_interface)),
      m_adp5587_keypad_i2c(bass_station::KeypadManager(ad5587_keypad_i2c, debounce_timer)),
      m_synth_control_switch(adg2188::Driver(adg2188_control_sw_i2c)),
      m_led_manager(bass_station::LedManager(led_spi_interface)),
      m_midi_driver(midi_usart_interface),
      m_debounce_timer(*debounce_timer)
{

#if not defined(X86_UNIT_TESTING_ONLY)

  // set the rotary encoder to a default value or "tempo"
  m_sequencer_encoder_timer.CNT = 16;
  // enable the rotary encoder (timer)
  m_sequencer_encoder_timer.CR1 = m_sequencer_encoder_timer.CR1 | TIM_CR1_CEN;

  // send the initial LED sequence to the TL5955 driver (this is normally called repeatedly in
  // execute_next_sequence_step())
  m_led_manager.set_both_rows_with_step_sequence_mapping(m_sequencer_step_map);

#endif
}

void SequenceManager::main_loop()
{
#if LED_TEST
  led_demo();
#endif
#if SEQUENCER_AUTOSTART_ON_BOOT

  // enable the tempo timer with update interrupt
  m_tempo_timer_device.DIER = m_tempo_timer_device.DIER | TIM_DIER_UIE;
  m_tempo_timer_device.CR1  = m_tempo_timer_device.CR1 | TIM_CR1_CEN;

  m_midi_state      = SequencerState::RUNNING;
  m_sequencer_state = SequencerState::RUNNING;

  // start the midi device early so that it synchronizes correctly
  m_midi_driver.send_realtime_start_msg();

#endif

  /// @brief main program infinite loop
  /// @return never
  while (true)
  {
    // probably needs its own timer callback as this will become less responsive at slower tempos
    update_display_and_tempo();

    // get latest key events from adp5587 (the sequencer pattern button presses (m_sequencer_step_map) and the user
    // start/stop buttons (return))
    SequencerState current_sequencer_state = m_adp5587_keypad_i2c.update_sequencer_map(m_sequencer_step_map);

    // update the midi running state/heartbeat
    switch (current_sequencer_state)
    {
      case SequencerState::RUNNING:

        // either recently booted or user reset the position with stop button
        if (m_sequence_position == 0)
        {
          // reset the 1/12 MIDI heartbeat count
          m_midi_driver.reset_midi_pulse_cnt();

          // tell MIDI slave device to start its pattern from beginning (restart)
          m_midi_driver.send_realtime_start_msg();

          // enable the timer with update interrupt
          m_tempo_timer_device.DIER = m_tempo_timer_device.DIER | TIM_DIER_UIE;
          m_tempo_timer_device.CR1  = m_tempo_timer_device.CR1 | TIM_CR1_CEN;

          m_midi_state      = SequencerState::RUNNING;
          m_sequencer_state = SequencerState::RUNNING;
        }
        else // resume/continue
        {
          // NOTE: to avoid MIDI/Sequencer sync issues, we don't reset the 1/12 MIDI heartbeat count on
          // continue/resume

          // tell MIDI slave device to continue its pattern from where it was stopped (resume)
          m_midi_driver.send_realtime_continue_msg();

          // enable the timer with update interrupt
          m_tempo_timer_device.DIER = m_tempo_timer_device.DIER | TIM_DIER_UIE;
          m_tempo_timer_device.CR1  = m_tempo_timer_device.CR1 | TIM_CR1_CEN;

          m_midi_state      = SequencerState::RUNNING;
          m_sequencer_state = SequencerState::RUNNING;
        }

        break;
      case SequencerState::STOPPED:

        // disable the timer with update interrupt
        m_tempo_timer_device.DIER = m_tempo_timer_device.DIER & ~TIM_DIER_UIE;
        m_tempo_timer_device.CR1  = m_tempo_timer_device.CR1 & ~TIM_CR1_CEN;

        // Tell the MIDI slave device to pause
        m_midi_driver.send_realtime_stop_msg();

        // silence any synth key/notes that are still sounding
        m_synth_control_switch.clear_all();

        // before state update, if sequencer state is already stopped reset pattern position
        if (m_sequencer_state == SequencerState::STOPPED)
        {
          m_sequence_position = 0;
        }

        // now update the states
        m_midi_state      = SequencerState::STOPPED;
        m_sequencer_state = SequencerState::STOPPED;

        break;
      case SequencerState::IDLE:
        // do nothing
        break;
    }

    // update the pattern LEDs and trigger synth key/note if running
    increment_sequencer();
  }
}

void SequenceManager::tempo_timer_isr()
{
  // update the pattern cursor once every 12 MIDI clock messages
  switch (m_midi_driver.get_midi_pulse_cnt())
  {
    default:
      // send the heartbeat clock signal to the MIDI OUT port
      m_midi_driver.send_realtime_clock_msg();
      m_midi_driver.increment_midi_pulse_cnt();
      break;
    case 12:
      m_midi_driver.reset_midi_pulse_cnt();
      // increment the step position in the pattern
      (m_sequence_position >= m_sequencer_key_mapping.size() - 1) ? m_sequence_position = 0 : m_sequence_position++;
      break;
  }

// reset the UIF bit to re-enable interrupts
#if not defined(X86_UNIT_TESTING_ONLY)
  m_tempo_timer_device.SR = m_tempo_timer_device.SR & ~TIM_SR_UIF;
  // LL_TIM_ClearFlag_UPDATE(m_tempo_timer_device.first);
#endif
}

void SequenceManager::rotary_sw_exti_isr()
{
  uint32_t timer_count_ms = m_debounce_timer.CNT;
  if (timer_count_ms - m_last_mode_debounce_count_ms > m_mode_debounce_threshold_ms)
  {
    if (m_current_mode == bass_station::SequenceManager::Mode::NOTE_SELECT)
    {
      m_current_mode = bass_station::SequenceManager::Mode::TEMPO_ADJUST;
      // restore the saved tempo value now we return to TEMPO_ADJUST mode
      m_sequencer_encoder_timer.CNT = m_saved_tempo_setting;
    }
    else
    {
      m_current_mode = bass_station::SequenceManager::Mode::NOTE_SELECT;
      // save the current tempo value whilst we are in NOTE_SELECT mode
      m_saved_tempo_setting = m_sequencer_encoder_timer.CNT;
    }
  }
  m_last_mode_debounce_count_ms = timer_count_ms;
}

void SequenceManager::update_display_and_tempo()
{

  if (m_current_mode == Mode::TEMPO_ADJUST)
  {
    // update the sequencer tempo (prescaler)
    // TODO rotary encoder is backwards: Should be CW = increase tempo, CCW = decrease tempo
    m_tempo_timer_device.PSC = m_sequencer_encoder_timer.CNT;

    noarch::containers::StaticString<20> mode_string("TEMPO MODE         ");

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);
  }
  else if (m_current_mode == Mode::NOTE_SELECT)
  {

    noarch::containers::StaticString<20> mode_string("NOTE MODE          ");

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);

    // lookup the step position using the index of the last user selected key
    /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
    Step last_selected_step      = m_sequencer_step_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second;
    Note last_selected_step_note = last_selected_step.m_note;

    // get the direction from the encoder and increment/decrement the note in the step of the last user selected key

    if (m_last_encoder_value != m_sequencer_encoder_timer.CNT)
    {
#if not defined(X86_UNIT_TESTING_ONLY)
      if (m_sequencer_encoder_timer.CR1 & TIM_CR1_DIR)
      // if (LL_TIM_GetDirection(m_sequencer_encoder_timer))
      {
        m_display_direction.concat(0, "up  ");
        /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
        m_sequencer_step_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note = static_cast<Note>(last_selected_step_note + 1);
      }
      else
      {

        m_display_direction.concat(0, "down");

        /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
        m_sequencer_step_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note = static_cast<Note>(last_selected_step_note - 1);
      }
#endif
    }
    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FOUR, m_display_direction);
    m_last_encoder_value = m_sequencer_encoder_timer.CNT;
  }

  // now read back the updated note from the step to get the note string value
  /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
  NoteData *lookup_note_data = m_note_switch_map.find_key(m_sequencer_step_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note);

  if (lookup_note_data != nullptr)
  {

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, lookup_note_data->m_note_static_string);
  }
  else
  {

    noarch::containers::StaticString<20> nullptr_text("---                ");

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, nullptr_text);
  }

  // update the display with the sequencer position index

  noarch::containers::StaticString<20> beat_pos;
  beat_pos.concat(0, "Position:");
  beat_pos.concat_int(9, m_sequence_position);

  m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);

  // update the display with the encoder count value (using the PSC as a shadow value if Mode::NOTE_SELECT)

  noarch::containers::StaticString<20> encoder_pos;
  encoder_pos.concat(0, "Tempo:");
  encoder_pos.concat_int(6, m_tempo_timer_device.PSC);

  m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);

  // volatile uint8_t tempo_timer_hz = 64000000 / (m_tempo_timer_device.first.PSC * m_tempo_timer_device.first.ARR);
  // volatile uint16_t tempo_timer_bpm = tempo_timer_hz * 60 / 10;
  // std::string tempo_string{""};
  // tempo_string += std::to_string(tempo_timer_bpm) + "   ";
  // m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, tempo_string);

  // redraw the display contents
  m_ssd1306_display_spi.update_oled();
}

void SequenceManager::increment_sequencer()
{

  // get the current sequence position Step object from the map
  // and save its current colour/state so it can be restored later
  Step &current_step = m_sequencer_step_map.data[m_sequencer_key_mapping[m_sequence_position]].second;

  tlc5955::LedColour previous_colour = current_step.m_colour;
  StepState previous_step_state      = current_step.m_state;

  // find the note for the enabled step so we can trigger the key/note on the synth
  if (current_step.m_state == StepState::ON)
  {
    // update LED colour to show the sequencer IS at this position in the pattern
    current_step.m_colour = beat_colour_on;

    NoteData *found_note_data = m_note_switch_map.find_key(current_step.m_note);

    // turn on/off the note sound from the previous step but only if sequencer is running
    if (m_sequencer_state == SequencerState::RUNNING)
    {
      //   first, turn off the synth key / note that we enabled on the previous pattern step if (m_previous_enabled_note
      if (m_previous_enabled_note != nullptr)
      {
        m_synth_control_switch.write_switch(adg2188::Driver::Throw::open, m_previous_enabled_note->m_sw, adg2188::Driver::Latch::set);
      }

      // second, turn on the synth key/note for the current step
      if (current_step.m_note != Note::none)
      {
        if (found_note_data != nullptr)
        {
          m_synth_control_switch.write_switch(adg2188::Driver::Throw::close, found_note_data->m_sw, adg2188::Driver::Latch::set);
        }
      }
    }

    // retain the synth key/note we enabled for this Step
    // so we can turn it off when we get to the next Step
    m_previous_enabled_note = found_note_data;
  }
  else // the current pattern Step is disabled. Disable the previous LED and synth key/note
  {
    // update LED colour to show the sequencer is NOT at this position in the pattern anymore
    current_step.m_colour = beat_colour_off;

    // turn off the note sound from the previous step
    if (m_previous_enabled_note != nullptr)
    {
      m_synth_control_switch.write_switch(adg2188::Driver::Throw::open, m_previous_enabled_note->m_sw, adg2188::Driver::Latch::set);
    }
  }

  // finally enable the current step in the sequence
  current_step.m_state = StepState::ON;

  // send the updated LED sequence map to the TL5955 driver
  m_led_manager.set_both_rows_with_step_sequence_mapping(m_sequencer_step_map);

  // restore the state of the current step (so it is cleared on the next iteration)
  current_step.m_colour = previous_colour;
  current_step.m_state  = previous_step_state;
}

// clang-format off
std::array< std::pair< Note, NoteData>, 25> SequenceManager::m_note_switch_data = {{
    { Note::c0,       NoteData("C0 ", adg2188::Driver::Pole::x4_to_y0) },
    { Note::c0_sharp, NoteData("C0#", adg2188::Driver::Pole::x5_to_y0) },
    { Note::d0,       NoteData("D0 ", adg2188::Driver::Pole::x6_to_y0) },
    { Note::d0_sharp, NoteData("D0#", adg2188::Driver::Pole::x7_to_y0) },
    { Note::e0,       NoteData("E0 ", adg2188::Driver::Pole::x0_to_y2) },
    { Note::f0,       NoteData("F0 ", adg2188::Driver::Pole::x1_to_y2) },
    { Note::f0_sharp, NoteData("F0#", adg2188::Driver::Pole::x2_to_y2) },
    { Note::g0,       NoteData("G0 ", adg2188::Driver::Pole::x3_to_y2) },
    { Note::g0_sharp, NoteData("G0#", adg2188::Driver::Pole::x4_to_y2) },
    { Note::a1,       NoteData("A1 ", adg2188::Driver::Pole::x5_to_y2) },
    { Note::a1_sharp, NoteData("A1#", adg2188::Driver::Pole::x6_to_y2) },
    { Note::b1,       NoteData("B1 ", adg2188::Driver::Pole::x7_to_y2) },
    { Note::c1,       NoteData("C1 ", adg2188::Driver::Pole::x0_to_y4) },  // Middle C
    { Note::c1_sharp, NoteData("C1#", adg2188::Driver::Pole::x1_to_y4) },
    { Note::d1,       NoteData("D1 ", adg2188::Driver::Pole::x2_to_y4) },
    { Note::d1_sharp, NoteData("D1#", adg2188::Driver::Pole::x3_to_y4) },
    { Note::e1,       NoteData("E1 ", adg2188::Driver::Pole::x4_to_y4) },
    { Note::f1,       NoteData("F1 ", adg2188::Driver::Pole::x5_to_y4) },
    { Note::f1_sharp, NoteData("F1#", adg2188::Driver::Pole::x6_to_y4) },
    { Note::g1,       NoteData("G1 ", adg2188::Driver::Pole::x7_to_y4) },
    { Note::g1_sharp, NoteData("G1#", adg2188::Driver::Pole::x0_to_y6) },
    { Note::a2,       NoteData("A2 ", adg2188::Driver::Pole::x1_to_y6) },
    { Note::a2_sharp, NoteData("A2#", adg2188::Driver::Pole::x2_to_y6) },
    { Note::b2,       NoteData("B2 ", adg2188::Driver::Pole::x3_to_y6) },
    { Note::c2,       NoteData("C2 ", adg2188::Driver::Pole::x4_to_y6) }
}};

// Size: 2K
// The default sequencer pattern, stored in SequencerManager::m_sequencer_step_map (noarch::containers::StaticMap)
std::array< std::pair< SequencerKeyEventIndex, Step >, 32 > SequenceManager::m_sequencer_step_data = {{
    {SequencerKeyEventIndex::A0_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON,  Note::c0, default_colour,         0,   4,  0)},
    {SequencerKeyEventIndex::A1_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::c0_sharp, default_colour,   1,   0,  1)},
    {SequencerKeyEventIndex::A2_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::d0, default_colour,         2,   5,  2)},
    {SequencerKeyEventIndex::A3_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::d0_sharp, default_colour,   3,   1,  3)},
    {SequencerKeyEventIndex::A4_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::e0, default_colour,         4,   2,  4)},
    {SequencerKeyEventIndex::A5_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::f0, default_colour,         5,   6,  5)},
    {SequencerKeyEventIndex::A6_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::f0_sharp, default_colour,   6,   3,  6)},
    {SequencerKeyEventIndex::A7_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::g0, default_colour,         7,   7,  7)},

    {SequencerKeyEventIndex::B0_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::e1, default_colour,         8,   11, 8)},
    {SequencerKeyEventIndex::B1_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         9,   15, 9)},
    {SequencerKeyEventIndex::B2_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c0, default_colour,         10,  10, 10)},
    {SequencerKeyEventIndex::B3_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         11,  14, 11)},
    {SequencerKeyEventIndex::B4_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c2, default_colour,         12,  13, 12)},
    {SequencerKeyEventIndex::B5_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         13,  9,  13)},
    {SequencerKeyEventIndex::B6_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c0, default_colour,         14,  12, 14)},
    {SequencerKeyEventIndex::B7_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         15,  8,  15)}, 

    {SequencerKeyEventIndex::C0_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::e0, default_colour,          0,   7,  16)},
    {SequencerKeyEventIndex::C1_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::f1, default_colour,         1,   3,  17)},
    {SequencerKeyEventIndex::C2_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::f1_sharp, default_colour,   2,   6,  18)},
    {SequencerKeyEventIndex::C3_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::g1, default_colour,         3,   2,  19)},
    {SequencerKeyEventIndex::C4_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::g1_sharp, default_colour,   4,   1,  20)},
    {SequencerKeyEventIndex::C5_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::a2, default_colour,         5,   5,  21)},
    {SequencerKeyEventIndex::C6_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::a2_sharp, default_colour,   6,   0,  22)},
    {SequencerKeyEventIndex::C7_OFF | SequencerKeyEventIndex::ON, Step(StepState::OFF, Note::b2, default_colour,         7,   4,  23)},  

    {SequencerKeyEventIndex::D0_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c2, default_colour,         8,   8,  24)},
    {SequencerKeyEventIndex::D1_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         9,   12, 25)},
    {SequencerKeyEventIndex::D2_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c0, default_colour,         10,  9,  26)},
    {SequencerKeyEventIndex::D3_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         11,  13, 27)},
    {SequencerKeyEventIndex::D4_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c2, default_colour,         12,  14, 28)},
    {SequencerKeyEventIndex::D5_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         13,  10, 29)},
    {SequencerKeyEventIndex::D6_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c0, default_colour,         14,  15, 30)},
    {SequencerKeyEventIndex::D7_OFF | SequencerKeyEventIndex::ON, Step(StepState::ON, Note::c1, default_colour,         15,  11, 31)},       
}};
// clang-format on

void SequenceManager::led_demo()
{
  uint32_t sweep_delay    = 20;
  uint32_t solid_delay    = 250;
  uint32_t flash_delay    = 1000;
  uint16_t fade_step      = 1024;
  uint16_t _pwm_led_value = 0;
  while (true)
  {

    // pwm fade
    m_led_manager.reinit_driver(tlc5955::Driver::DisplayFunction::display_repeat_off,
                                tlc5955::Driver::TimingFunction::timing_reset_on,
                                tlc5955::Driver::RefreshFunction::auto_refresh_off,
                                tlc5955::Driver::PwmFunction::enhanced_pwm);

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    }

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    }

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::cyan);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::cyan);
    }

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::blue);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::blue);
    }

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::yellow);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::yellow);
    }

    for (_pwm_led_value = 0; _pwm_led_value < 32768; _pwm_led_value += fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::green);
    }
    for (_pwm_led_value = 32768; _pwm_led_value > 0; _pwm_led_value -= fade_step)
    {
      m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::green);
    }

    // sweep
    m_led_manager.reinit_driver(tlc5955::Driver::DisplayFunction::display_repeat_off,
                                tlc5955::Driver::TimingFunction::timing_reset_on,
                                tlc5955::Driver::RefreshFunction::auto_refresh_off,
                                tlc5955::Driver::PwmFunction::enhanced_pwm);

    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::red, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::magenta, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::cyan, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::blue, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::green, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::yellow, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::red, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::magenta, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::cyan, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::blue, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::green, sweep_delay);
    m_led_manager.run_led_sweep(m_sequencer_step_map, tlc5955::LedColour::yellow, sweep_delay);

    // colour cycle
    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 8;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::cyan);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::blue);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::green);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::yellow);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::cyan);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::blue);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::green);
    stm32::delay_millisecond(solid_delay);
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::yellow);
    stm32::delay_millisecond(solid_delay);

    // pwm flash
    m_led_manager.reinit_driver(tlc5955::Driver::DisplayFunction::display_repeat_off,
                                tlc5955::Driver::TimingFunction::timing_reset_off,
                                tlc5955::Driver::RefreshFunction::auto_refresh_off,
                                tlc5955::Driver::PwmFunction::enhanced_pwm);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 1;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 2;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 3;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::cyan);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 4;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::blue);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 5;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::green);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 6;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::yellow);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 7;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 8;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::red);
    stm32::delay_millisecond(flash_delay);

    _pwm_led_value = (std::numeric_limits<uint16_t>::max() / 8) * 8;
    m_led_manager.set_all_leds_both_rows(_pwm_led_value, tlc5955::LedColour::magenta);
    stm32::delay_millisecond(flash_delay);
  }
}

} // namespace bass_station