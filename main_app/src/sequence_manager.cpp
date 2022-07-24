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

/// @brief Cycle sequencer LEDs through primary/secondary colours. Warning, this will disable normal sequencer function.
#define LED_TEST 1
/// @brief Automatically start the sequencer on startup. No user input required.
#define SEQUENCER_AUTOSTART_ON_BOOT 1

namespace bass_station
{

SequenceManager::SequenceManager(std::pair<TIM_TypeDef *, STM32G0_ISR> tempo_timer_pair,
                                 TIM_TypeDef *sequencer_encoder_timer,
                                 ssd1306::DriverSerialInterface<STM32G0_ISR> &display_spi_interface,
                                 I2C_TypeDef *ad5587_keypad_i2c, TIM_TypeDef *debounce_timer,
                                 I2C_TypeDef *adg2188_control_sw_i2c, tlc5955::DriverSerialInterface &led_spi_interface,
                                 midi_stm32::DeviceInterface<STM32G0_ISR> &midi_usart_interface)

    : m_tempo_timer_pair(tempo_timer_pair), m_sequencer_encoder_timer(sequencer_encoder_timer),
      m_ssd1306_display_spi(bass_station::DisplayManager(display_spi_interface)),
      m_adp5587_keypad_i2c(bass_station::KeypadManager(ad5587_keypad_i2c, debounce_timer)),
      m_synth_control_switch(adg2188::Driver(adg2188_control_sw_i2c)),
      m_led_manager(bass_station::LedManager(led_spi_interface)), m_midi_driver(midi_usart_interface),
      m_debounce_timer(debounce_timer)
{

#if not defined(X86_UNIT_TESTING_ONLY)

    // set the rotary encoder to a default value or "tempo"
    m_sequencer_encoder_timer->CNT = 16;
    // enable the rotary encoder (timer)
    m_sequencer_encoder_timer->CR1 = m_sequencer_encoder_timer->CR1 | TIM_CR1_CEN;
    // setup rotary encoder switch callback (rotary_sw_exti_isr()) to allow mode change
    // (bass_station::SequenceManager::Mode)
    m_rotary_sw_exti_handler.init_rotary_encoder_callback(this);

    // setup tempo timer callback to allow pattern sequence update
    m_sequencer_tempo_timer_isr_handler.init_tempo_timer_callback(this);

    // send the initial LED sequence to the TL5955 driver (this is normally called repeatedly in
    // execute_next_sequence_step())
    m_led_manager.send_both_rows_greyscale_data(m_sequence_map);

#endif
}

void SequenceManager::main_loop()
{
#if LED_TEST
    uint16_t _pwm_led_value = std::numeric_limits<uint16_t>::max();
    while (true)
    {

        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::red);
        stm32::delay_millisecond(2000);
        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::magenta);
        stm32::delay_millisecond(2000);
        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::blue);
        stm32::delay_millisecond(2000);
        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::cyan);
        stm32::delay_millisecond(2000);
        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::yellow);
        stm32::delay_millisecond(2000);
        m_led_manager.set_all_leds(_pwm_led_value, bass_station::LedColour::green);
        stm32::delay_millisecond(2000);
    }
#endif
#if SEQUENCER_AUTOSTART_ON_BOOT

    // enable the tempo timer with update interrupt
    m_tempo_timer_pair.first->DIER = m_tempo_timer_pair.first->DIER | TIM_DIER_UIE;
    m_tempo_timer_pair.first->CR1 = m_tempo_timer_pair.first->CR1 | TIM_CR1_CEN;

    m_midi_state = UserKeyStates::RUNNING;
    m_sequencer_state = UserKeyStates::RUNNING;

    // start the midi device early so that it synchronizes correctly
    m_midi_driver.send_realtime_start_msg();

#endif

    /// @brief main program infinite loop
    /// @return never
    while (true)
    {
        // probably needs its own timer callback as this will become less responsive at slower tempos
        update_display_and_tempo();

        // redraw the display contents
        m_ssd1306_display_spi.update_oled();

        // get latest key events from adp5587 (the sequencer pattern button presses (m_sequence_map) and the user
        // start/stop buttons (return))
        UserKeyStates new_state = m_adp5587_keypad_i2c.process_key_events(m_sequence_map);

        // update the midi running state/heartbeat
        switch (new_state)
        {
        case UserKeyStates::RUNNING:

            // either justed booted or user reset the position with stop button
            if (m_pattern_cursor == 0)
            {
                // reset the 1/12 MIDI heartbeat count
                m_midi_driver.reset_midi_pulse_cnt();

                // tell MIDI slave device to start its pattern from beginning (restart)
                m_midi_driver.send_realtime_start_msg();

                // enable the timer with update interrupt
                m_tempo_timer_pair.first->DIER = m_tempo_timer_pair.first->DIER | TIM_DIER_UIE;
                m_tempo_timer_pair.first->CR1 = m_tempo_timer_pair.first->CR1 | TIM_CR1_CEN;

                m_midi_state = UserKeyStates::RUNNING;
                m_sequencer_state = UserKeyStates::RUNNING;
            }
            else // resume/continue
            {
                // NOTE: to avoid MIDI/Sequencer sync issues, we don't reset the 1/12 MIDI heartbeat count on
                // continue/resume

                // tell MIDI slave device to continue its pattern from where it was stopped (resume)
                m_midi_driver.send_realtime_continue_msg();

                // enable the timer with update interrupt
                m_tempo_timer_pair.first->DIER = m_tempo_timer_pair.first->DIER | TIM_DIER_UIE;
                m_tempo_timer_pair.first->CR1 = m_tempo_timer_pair.first->CR1 | TIM_CR1_CEN;

                m_midi_state = UserKeyStates::RUNNING;
                m_sequencer_state = UserKeyStates::RUNNING;
            }

            break;
        case UserKeyStates::STOPPED:

            // disable the timer with update interrupt
            m_tempo_timer_pair.first->DIER = m_tempo_timer_pair.first->DIER & ~TIM_DIER_UIE;
            m_tempo_timer_pair.first->CR1 = m_tempo_timer_pair.first->CR1 & ~TIM_CR1_CEN;

            // Tell the MIDI slave device to pause
            m_midi_driver.send_realtime_stop_msg();

            // silence any synth key/notes that are still sounding
            m_synth_control_switch.clear_all();

            // before state update, if sequencer state is already stopped reset pattern position
            if (m_sequencer_state == UserKeyStates::STOPPED)
            {
                m_pattern_cursor = 0;
            }

            // now update the states
            m_midi_state = UserKeyStates::STOPPED;
            m_sequencer_state = UserKeyStates::STOPPED;

            break;
        case UserKeyStates::IDLE:
            // do nothing
            break;
        }

        // update the pattern LEDs and trigger synth key/note if running
        execute_next_sequence_step();
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
        (m_pattern_cursor >= m_sequencer_key_mapping.size() - 1) ? m_pattern_cursor = 0 : m_pattern_cursor++;
        break;
    }

// reset the UIF bit to re-enable interrupts
#if not defined(X86_UNIT_TESTING_ONLY)
    m_tempo_timer_pair.first->SR = m_tempo_timer_pair.first->SR & ~TIM_SR_UIF;
    // LL_TIM_ClearFlag_UPDATE(m_tempo_timer_pair.first);
#endif
}

void SequenceManager::rotary_sw_exti_isr()
{
    uint32_t timer_count_ms = m_debounce_timer->CNT;
    if (timer_count_ms - m_last_mode_debounce_count_ms > m_mode_debounce_threshold_ms)
    {
        if (m_current_mode == bass_station::SequenceManager::Mode::NOTE_SELECT)
        {
            m_current_mode = bass_station::SequenceManager::Mode::TEMPO_ADJUST;
            // restore the saved tempo value now we return to TEMPO_ADJUST mode
            m_sequencer_encoder_timer->CNT = m_saved_tempo_setting;
        }
        else
        {
            m_current_mode = bass_station::SequenceManager::Mode::NOTE_SELECT;
            // save the current tempo value whilst we are in NOTE_SELECT mode
            m_saved_tempo_setting = m_sequencer_encoder_timer->CNT;
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
        m_tempo_timer_pair.first->PSC = m_sequencer_encoder_timer->CNT;
#ifdef USE_STD_STRING
        std::string mode_string{""};
        mode_string += "TEMPO MODE";
#else
        noarch::containers::StaticString<20> mode_string("TEMPO MODE         ");
#endif
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);
    }
    else if (m_current_mode == Mode::NOTE_SELECT)
    {
#ifdef USE_STD_STRING
        std::string mode_string{""};
        mode_string += "NOTE MODE ";
#else
        noarch::containers::StaticString<20> mode_string("NOTE MODE          ");
#endif
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);

        // lookup the step position using the index of the last user selected key
        /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
        Step last_selected_step = m_sequence_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second;
        Note last_selected_step_note = last_selected_step.m_note;

        // get the direction from the encoder and increment/decrement the note in the step of the last user selected key

        if (m_last_encoder_value != m_sequencer_encoder_timer->CNT)
        {
#if not defined(X86_UNIT_TESTING_ONLY)
            if (m_sequencer_encoder_timer->CR1 & TIM_CR1_DIR)
            // if (LL_TIM_GetDirection(m_sequencer_encoder_timer))
            {
#ifdef USE_STD_STRING
                m_display_direction += "up  ";
#else

                m_display_direction.concat(0, "up  ");
#endif
                /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
                m_sequence_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note =
                    static_cast<Note>(last_selected_step_note + 1);
            }
            else
            {
#ifdef USE_STD_STRING
                m_display_direction += "DOWN";
#else
                m_display_direction.concat(0, "down");
#endif
                /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
                m_sequence_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note =
                    static_cast<Note>(last_selected_step_note - 1);
            }
#endif
        }
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FOUR, m_display_direction);
        m_last_encoder_value = m_sequencer_encoder_timer->CNT;
    }

    // now read back the updated note from the step to get the note string value
    /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
    NoteData *lookup_note_data =
        m_note_switch_map.find_key(m_sequence_map.data[m_adp5587_keypad_i2c.last_user_selected_key_idx].second.m_note);

    if (lookup_note_data != nullptr)
    {
#ifdef USE_STD_STRING
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, lookup_note_data->m_note_string);
#else
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE,
                                               lookup_note_data->m_note_static_string);
#endif
    }
    else
    {
#ifdef USE_STD_STRING
        std::string nullptr_text{"---"};
#else
        noarch::containers::StaticString<20> nullptr_text("---                ");
#endif
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, nullptr_text);
    }

// update the display with the sequencer position index
#ifdef USE_STD_STRING
    std::string beat_pos{"Position: "};
    beat_pos += std::to_string(m_pattern_cursor) + ' ';
#else
    noarch::containers::StaticString<20> beat_pos;
    beat_pos.concat(0, "Position:");
    beat_pos.concat_int(9, m_pattern_cursor);
#endif

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);

// update the display with the encoder count value (using the PSC as a shadow value if Mode::NOTE_SELECT)
#ifdef USE_STD_STRING
    std::string encoder_pos{"Tempo: "};
    encoder_pos += std::to_string(m_tempo_timer_pair.first->PSC) + "   ";
#else
    noarch::containers::StaticString<20> encoder_pos;
    encoder_pos.concat(0, "Tempo:");
    encoder_pos.concat_int(6, m_tempo_timer_pair.first->PSC);
#endif

    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);

    // volatile uint8_t tempo_timer_hz = 64000000 / (m_tempo_timer_pair.first->PSC * m_tempo_timer_pair.first->ARR);
    // volatile uint16_t tempo_timer_bpm = tempo_timer_hz * 60 / 10;
    // std::string tempo_string{""};
    // tempo_string += std::to_string(tempo_timer_bpm) + "   ";
    // m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, tempo_string);
}

void SequenceManager::execute_next_sequence_step()
{

    // get the current sequence position Step object from the map
    // and save its current colour/state so it can be restored later
    /// @note don't use std::array.at(), this will force exception handling to bloat the linked .elf
    Step &current_step = m_sequence_map.data[m_sequencer_key_mapping[m_pattern_cursor]].second;

    LedColour previous_colour = current_step.m_colour;
    KeyState previous_key_state = current_step.m_key_state;

    // find the note for the enabled step so we can trigger the key/note on the synth
    if (current_step.m_key_state == KeyState::ON)
    {
        // update LED colour to show the sequencer IS at this position in the pattern
        current_step.m_colour = beat_colour_on;

        NoteData *found_note_data = m_note_switch_map.find_key(current_step.m_note);

        // turn on/off the note sound from the previous step but only if sequencer is running
        if (m_sequencer_state == UserKeyStates::RUNNING)
        {
            // first, turn off the synth key/note that we enabled on the previous pattern step
            // if (m_previous_enabled_note != nullptr)
            // {
            //     m_synth_control_switch.write_switch(
            //         adg2188::Driver::Throw::open,
            //         m_previous_enabled_note->m_sw,
            //         adg2188::Driver::Latch::set);
            // }

            // // second, turn on the synth key/note for the current step
            // if (current_step.m_note != Note::none)
            // {
            //     if (found_note_data != nullptr)
            //     {
            //         m_synth_control_switch.write_switch(
            //             adg2188::Driver::Throw::close,
            //             found_note_data->m_sw,
            //             adg2188::Driver::Latch::set);
            //     }

            // }
        }

        // retain the synth key/note we enabled for this Step
        // so we can turn it off when we get to the next Step
        m_previous_enabled_note = found_note_data;
    }
    else // the current pattern Step is disabled. Disable the previous LED and synth key/note
    {
        // update LED colour to show the sequencer is NOT at this position in the pattern anymore
        current_step.m_colour = beat_colour_off;

        // // turn off the note sound from the previous step
        // if (m_previous_enabled_note != nullptr)
        // {
        //     m_synth_control_switch.write_switch(
        //         adg2188::Driver::Throw::open,
        //         m_previous_enabled_note->m_sw,
        //         adg2188::Driver::Latch::set);
        // }
    }

    // finally enable the current step in the sequence
    current_step.m_key_state = KeyState::ON;

    // send the updated LED sequence map to the TL5955 driver
    m_led_manager.send_both_rows_greyscale_data(m_sequence_map);

    // restore the state of the current step (so it is cleared on the next iteration)
    current_step.m_colour = previous_colour;
    current_step.m_key_state = previous_key_state;
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
// The default sequencer pattern, stored in SequencerManager::m_sequence_map (noarch::containers::StaticMap)
std::array< std::pair< adp5587::Driver<STM32G0_ISR>::KeyPadMappings, Step >, 32 > SequenceManager::m_sequence_data = {{
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A0_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON,  Note::c0, default_colour,         0,   4,  0)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A1_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::c0_sharp, default_colour,   1,   0,  1)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A2_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::d0, default_colour,         2,   5,  2)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A3_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::d0_sharp, default_colour,   3,   1,  3)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A4_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::e0, default_colour,         4,   2,  4)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A5_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f0, default_colour,         5,   6,  5)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A6_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f0_sharp, default_colour,   6,   3,  6)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::A7_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g0, default_colour,         7,   7,  7)},

    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B0_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::e1, default_colour,       8,   11, 8)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B1_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         9,   15, 9)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B2_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         10,  10, 10)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B3_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         11,  14, 11)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B4_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         12,  13, 12)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B5_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         13,  9,  13)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B6_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         14,  12, 14)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::B7_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         15,  8,  15)}, 

    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C0_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::e0, default_colour,         0,   7,  16)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C1_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f1, default_colour,         1,   3,  17)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C2_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f1_sharp, default_colour,   2,   6,  18)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C3_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g1, default_colour,         3,   2,  19)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C4_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g1_sharp, default_colour,   4,   1,  20)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C5_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::a2, default_colour,         5,   5,  21)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C6_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::a2_sharp, default_colour,   6,   0,  22)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::C7_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::OFF, Note::b2, default_colour,         7,   4,  23)},  

    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D0_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         8,   8,  24)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D1_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         9,   12, 25)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D2_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         10,  9,  26)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D3_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         11,  13, 27)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D4_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         12,  14, 28)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D5_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         13,  10, 29)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D6_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         14,  15, 30)},
    {adp5587::Driver<STM32G0_ISR>::KeyPadMappings::D7_OFF | adp5587::Driver<STM32G0_ISR>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         15,  11, 31)},       
}};
// clang-format on

} // namespace bass_station