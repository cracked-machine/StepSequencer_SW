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

#include <sequence_manager.hpp>

namespace bass_station
{

SequenceManager::SequenceManager(
    tempo_timer_pair_t &tempo_timer_pair,
    TIM_TypeDef *sequencer_encoder_timer,
    ssd1306::DriverSerialInterface<stm32::isr::InterruptTypeStm32g0> &display_spi_interface, 
    I2C_TypeDef *ad5587_keypad_i2c,
    TIM_TypeDef *debounce_timer,
    I2C_TypeDef *adg2188_control_sw_i2c,
    tlc5955::DriverSerialInterface &led_spi_interface,
    midi_stm32::DeviceInterface<stm32::isr::InterruptTypeStm32g0> &midi_usart_interface) 
    
    :   m_tempo_timer_pair(tempo_timer_pair),
        m_sequencer_encoder_timer(sequencer_encoder_timer),
        m_ssd1306_display_spi(bass_station::DisplayManager(display_spi_interface)),
        m_adp5587_keypad_i2c(bass_station::KeypadManager(ad5587_keypad_i2c, debounce_timer)),
        m_synth_control_switch(adg2188::Driver(adg2188_control_sw_i2c)),
        m_led_manager(bass_station::LedManager(led_spi_interface)),
        m_midi_driver(midi_usart_interface),
        m_debounce_timer(debounce_timer)
{

    // send configuration data to TLC5955
    m_led_manager.send_control_data();

    // enable the timer with a starting "tempo"
    m_sequencer_encoder_timer->CNT = 16;

    #if not defined(X86_UNIT_TESTING_ONLY)
        LL_TIM_EnableCounter(m_sequencer_encoder_timer.get());

        // setup this class as timer ISR handler
        m_sequencer_tempo_timer_isr_handler.initialise(this);

        // setup this class as EXTI ISR handler
        m_rotary_sw_exti_handler.register_driver(this);

        LL_TIM_EnableIT_UPDATE(m_tempo_timer_pair.first);  
        LL_TIM_EnableCounter(m_tempo_timer_pair.first);        
        
    #endif
}

void SequenceManager::start_loop()
{
    // start connected MIDI device now and start timer to send heartbeat 
    m_midi_driver.send_realtime_start_msg();
    
    while(true)
    {
        // probably needs its own timer callback as this will become less responsive at slower tempos
        update_display_and_tempo();

        // redraw the display contents
        m_ssd1306_display_spi.update_oled();

        // get latest key events from adp5587 (the sequencer pattern button presses (m_sequence_map) and the user start/stop buttons (return))
        UserKeyStates new_state = m_adp5587_keypad_i2c.process_key_events(m_sequence_map);
        
        // update the midi running state/heartbeat 
        switch(new_state)
        {
            case UserKeyStates::RUNNING:
                
                // reset the sequence pattern position when we press play
                m_pattern_cursor = 0;
                m_midi_driver.reset_midi_pulse_cnt();
                // Tell the MIDI slave device to start from beginning
                m_midi_driver.send_realtime_start_msg();
                LL_TIM_EnableIT_UPDATE(m_tempo_timer_pair.first);  
                LL_TIM_EnableCounter(m_tempo_timer_pair.first);    

                m_midi_state = UserKeyStates::RUNNING;  
                m_sequencer_state = UserKeyStates::RUNNING;  
                break;
            case UserKeyStates::STOPPED:
     
                // Tell the MIDI slave device to pause
                m_midi_driver.send_realtime_stop_msg();
                LL_TIM_DisableIT_UPDATE(m_tempo_timer_pair.first);  
                LL_TIM_DisableCounter(m_tempo_timer_pair.first);                    

                m_midi_state = UserKeyStates::STOPPED;
                m_sequencer_state = UserKeyStates::STOPPED;  
                break;
            case UserKeyStates::IDLE:
                // execute_next_sequence_step(); 
                break;
        }        

    
        // update the sequencer running state/tempo timer
        switch(m_sequencer_state)
        {
            case UserKeyStates::RUNNING:
                execute_next_sequence_step();
                break;
            default:      
                break;
        }
    }
}

void SequenceManager::tempo_timer_isr()
{
    // update the pattern cursor once every 12 MIDI clock messages
    switch(m_midi_driver.get_midi_pulse_cnt())
    {
        default:
            // send the heartbeat clock signal to the MIDI OUT port
            m_midi_driver.send_realtime_clock_msg();
            m_midi_driver.increment_midi_pulse_cnt();
            break;
        case 12:
            m_midi_driver.reset_midi_pulse_cnt();
            // increment the step position in the pattern
            (m_pattern_cursor >= m_sequencer_key_mapping.size() -1) ? m_pattern_cursor = 0: m_pattern_cursor++;
            break;            
    }

    // reset the UIF bit to re-enable interrupts
    #if not defined(X86_UNIT_TESTING_ONLY)
        LL_TIM_ClearFlag_UPDATE(m_tempo_timer_pair.first);
    #endif
}

void SequenceManager::rotary_sw_exti_isr()
{
    uint32_t timer_count_ms = LL_TIM_GetCounter(TIM17);
    if (timer_count_ms - m_last_mode_debounce_count_ms > m_mode_debounce_threshold_ms) 
    {
        if (m_current_mode == bass_station::SequenceManager::Mode::NOTE_SELECT) 
        { 
            m_current_mode = bass_station::SequenceManager::Mode::TEMPO_ADJUST; 
            // restore the saved tempo value now we return to TEMPO_ADJUST mode
            m_sequencer_encoder_timer->CNT = m_saved_tempo_setting;
            
        }
        else { 
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
        m_tempo_timer_pair.first->PSC =  m_sequencer_encoder_timer->CNT;
        std::string mode_string{""};
        mode_string += "TEMPO MODE";
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);        
    }
    else if (m_current_mode == Mode::NOTE_SELECT)
    {
        std::string mode_string{""};
        mode_string += "NOTE MODE ";
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, mode_string);   

        // lookup the step position using the index of the last user selected key
        Step last_selected_step = m_sequence_map.data.at(m_adp5587_keypad_i2c.last_user_selected_key_idx).second;
        [[maybe_unused]] Note last_selected_step_note = last_selected_step.m_note ;
        
        // get the direction from the encoder and increment/decrement the note in the step of the last user selected key
        std::string direction{""};
        if (m_last_encoder_value != m_sequencer_encoder_timer->CNT)
        {
            #if not defined(X86_UNIT_TESTING_ONLY)
                if (LL_TIM_GetDirection(m_sequencer_encoder_timer.get()))
                {
                    direction += "up  ";
                    m_sequence_map.data.at(m_adp5587_keypad_i2c.last_user_selected_key_idx).second.m_note = 
                        static_cast<Note>(last_selected_step_note + 1);
                }
                else
                {
                    direction += "down";
                    m_sequence_map.data.at(m_adp5587_keypad_i2c.last_user_selected_key_idx).second.m_note = 
                        static_cast<Note>(last_selected_step_note - 1);                
                }
            #endif
        }
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FOUR, direction);
        m_last_encoder_value = m_sequencer_encoder_timer->CNT;
    }

    // now read back the updated note from the step to get the note string value
    NoteData *lookup_note_data = m_note_switch_map.find_key(m_sequence_map.data.at(m_adp5587_keypad_i2c.last_user_selected_key_idx).second.m_note);

    if (lookup_note_data != nullptr)
    {
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, lookup_note_data->m_note_string);
    }
    else
    {
        std::string nullptr_text{"---"};
        m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_FIVE, nullptr_text);
    }     

    // update the display with the sequencer position index
    std::string beat_pos{"Position: "};
    beat_pos += std::to_string(m_pattern_cursor) + ' ';
    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);
    
    // update the display with the encoder count value (using the PSC as a shadow value if Mode::NOTE_SELECT)
    std::string encoder_pos{"Tempo: "};
    encoder_pos += std::to_string(m_tempo_timer_pair.first->PSC) + "   ";
    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);        

    
    // volatile uint8_t tempo_timer_hz = 64000000 / (m_tempo_timer_pair.first->PSC * m_tempo_timer_pair.first->ARR);
    // volatile uint16_t tempo_timer_bpm = tempo_timer_hz * 60 / 10;
    // std::string tempo_string{""};
    // tempo_string += std::to_string(tempo_timer_bpm) + "   ";
    // m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_THREE, tempo_string);     
}

void SequenceManager::execute_next_sequence_step(bool run_demo_only)
{
    if (run_demo_only)
    {
        // this is kinda broken but it does something colourful
        m_led_manager.update_ladder_demo(m_sequence_map, 0xFFFF, 100);
    }   
    else
    {
        // get the Step object for the current sequence position
        Step &current_step = m_sequence_map.data.at(m_sequencer_key_mapping.at(m_pattern_cursor)).second;

        // save the colour and state of the current step so it can be restored later
        LedColour previous_colour = current_step.m_colour;
        KeyState previous_key_state = current_step.m_key_state;
        
        if (current_step.m_key_state == KeyState::ON)
        {
            NoteData *found_note_data = m_note_switch_map.find_key(current_step.m_note);

            // update LED colour
            current_step.m_colour = beat_colour_on;
            
            // turn off the note sound from the previous step
            if (m_previous_enabled_note != nullptr)
            {
                m_synth_control_switch.write_switch(
                    adg2188::Driver::Throw::open, 
                    m_previous_enabled_note->m_sw,
                    adg2188::Driver::Latch::set); 
            }

            if (current_step.m_note != Note::none)
            {

                if (found_note_data != nullptr)
                {
                    // turn on the note sound for the current step
                    m_synth_control_switch.write_switch(
                        adg2188::Driver::Throw::close, 
                        found_note_data->m_sw,
                        adg2188::Driver::Latch::set);                            
                }
                
            }    
            // retain the note we enabled this iteration so we can turn it off in the next iteration
            m_previous_enabled_note = found_note_data;                
        }
        else
        {
            // update the LED colour
            current_step.m_colour = beat_colour_off;

            // turn off the note sound from the previous step
            if (m_previous_enabled_note != nullptr)
            {
                m_synth_control_switch.write_switch(
                    adg2188::Driver::Throw::open, 
                    m_previous_enabled_note->m_sw,
                    adg2188::Driver::Latch::set); 
            }
        }    

        // enable the current step in the sequence
        current_step.m_key_state = KeyState::ON;

        // send the entire updated LED sequence to the TL5955 driver
        m_led_manager.send_both_rows_greyscale_data(m_sequence_map);
        
        // restore the state of the current step (so it is cleared on the next iteration)
        current_step.m_colour = previous_colour;
        current_step.m_key_state = previous_key_state;

        
    }
}

// @brief The keyboard notes of the BassStation and their associated control switch pole
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

// The default sequencer pattern, stored in SequencerManager::m_sequence_map (noarch::containers::StaticMap)
std::array< std::pair< adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings, Step >, 32 > SequenceManager::m_sequence_data = {{
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A0_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON,  Note::c0, default_colour,         0,   4,  0)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A1_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::c0_sharp, default_colour,   1,   0,  1)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A2_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::d0, default_colour,         2,   5,  2)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A3_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::d0_sharp, default_colour,   3,   1,  3)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A4_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::e0, default_colour,         4,   2,  4)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A5_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f0, default_colour,         5,   6,  5)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A6_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f0_sharp, default_colour,   6,   3,  6)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::A7_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g0, default_colour,         7,   7,  7)},

    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B0_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::e1, default_colour,       8,   11, 8)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B1_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         9,   15, 9)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B2_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         10,  10, 10)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B3_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         11,  14, 11)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B4_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         12,  13, 12)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B5_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         13,  9,  13)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B6_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         14,  12, 14)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::B7_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         15,  8,  15)}, 

    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C0_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::e0, default_colour,         0,   7,  16)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C1_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f1, default_colour,         1,   3,  17)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C2_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::f1_sharp, default_colour,   2,   6,  18)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C3_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g1, default_colour,         3,   2,  19)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C4_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::g1_sharp, default_colour,   4,   1,  20)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C5_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::a2, default_colour,         5,   5,  21)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C6_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::a2_sharp, default_colour,   6,   0,  22)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::C7_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::OFF, Note::b2, default_colour,         7,   4,  23)},  

    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D0_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         8,   8,  24)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D1_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         9,   12, 25)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D2_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         10,  9,  26)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D3_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         11,  13, 27)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D4_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c2, default_colour,         12,  14, 28)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D5_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         13,  10, 29)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D6_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c0, default_colour,         14,  15, 30)},
    {adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::D7_OFF | adp5587::Driver<stm32::isr::InterruptTypeStm32g0>::KeyPadMappings::ON, Step(KeyState::ON, Note::c1, default_colour,         15,  11, 31)},       
}};    


} // namespace bass_station