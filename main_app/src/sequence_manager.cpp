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

// The default sequencer pattern, stored in SequencerManager::m_sequence_map (noarch::containers::StaticMap)
std::array< std::pair< adp5587::Driver::KeyPadMappings, Step >, 32 > SequenceManager::sequence_data = {{
    {adp5587::Driver::KeyPadMappings::A0_ON, Step(KeyState::OFF, NoteSwitchMapping::c0, default_colour, 0,   4,  16)},
    {adp5587::Driver::KeyPadMappings::A1_ON, Step(KeyState::OFF, NoteSwitchMapping::c0_sharp, default_colour, 1,   0,  17)},
    {adp5587::Driver::KeyPadMappings::A2_ON, Step(KeyState::OFF, NoteSwitchMapping::d0, default_colour, 2,   5,  18)},
    {adp5587::Driver::KeyPadMappings::A3_ON, Step(KeyState::OFF, NoteSwitchMapping::d0_sharp, default_colour, 3,   1,  19)},
    {adp5587::Driver::KeyPadMappings::A4_ON, Step(KeyState::OFF, NoteSwitchMapping::e0, default_colour, 4,   2,  20)},
    {adp5587::Driver::KeyPadMappings::A5_ON, Step(KeyState::OFF, NoteSwitchMapping::f0, default_colour, 5,   6,  21)},
    {adp5587::Driver::KeyPadMappings::A6_ON, Step(KeyState::OFF, NoteSwitchMapping::f0_sharp, default_colour, 6,   3,  22)},
    {adp5587::Driver::KeyPadMappings::A7_ON, Step(KeyState::OFF, NoteSwitchMapping::g0, default_colour, 7,   7,  23)},

    {adp5587::Driver::KeyPadMappings::B0_ON, Step(KeyState::OFF, NoteSwitchMapping::c2, default_colour, 8,   11, 24)},
    {adp5587::Driver::KeyPadMappings::B1_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 9,   15, 25)},
    {adp5587::Driver::KeyPadMappings::B2_ON, Step(KeyState::OFF, NoteSwitchMapping::c0, default_colour, 10,  10, 26)},
    {adp5587::Driver::KeyPadMappings::B3_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 11,  14, 27)},
    {adp5587::Driver::KeyPadMappings::B4_ON, Step(KeyState::OFF, NoteSwitchMapping::c2, default_colour, 12,  13, 28)},
    {adp5587::Driver::KeyPadMappings::B5_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 13,  9,  29)},
    {adp5587::Driver::KeyPadMappings::B6_ON, Step(KeyState::OFF, NoteSwitchMapping::c0, default_colour, 14,  12, 30)},
    {adp5587::Driver::KeyPadMappings::B7_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 15,  8,  31)}, 

    {adp5587::Driver::KeyPadMappings::C0_ON, Step(KeyState::OFF, NoteSwitchMapping::e1, default_colour, 0,   7,  0)},
    {adp5587::Driver::KeyPadMappings::C1_ON, Step(KeyState::OFF, NoteSwitchMapping::f1, default_colour, 1,   3,  1)},
    {adp5587::Driver::KeyPadMappings::C2_ON, Step(KeyState::OFF, NoteSwitchMapping::f1_sharp, default_colour, 2,   6,  2)},
    {adp5587::Driver::KeyPadMappings::C3_ON, Step(KeyState::OFF, NoteSwitchMapping::g1, default_colour, 3,   2,  3)},
    {adp5587::Driver::KeyPadMappings::C4_ON, Step(KeyState::OFF, NoteSwitchMapping::g2_sharp, default_colour, 4,   1,  4)},
    {adp5587::Driver::KeyPadMappings::C5_ON, Step(KeyState::OFF, NoteSwitchMapping::a2, default_colour, 5,   5,  5)},
    {adp5587::Driver::KeyPadMappings::C6_ON, Step(KeyState::OFF, NoteSwitchMapping::a2_sharp, default_colour, 6,   0,  6)},
    {adp5587::Driver::KeyPadMappings::C7_ON, Step(KeyState::OFF, NoteSwitchMapping::b2, default_colour, 7,   4,  7)},  

    {adp5587::Driver::KeyPadMappings::D0_ON, Step(KeyState::OFF, NoteSwitchMapping::c2, default_colour, 8,   8,  8)},
    {adp5587::Driver::KeyPadMappings::D1_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 9,   12, 9)},
    {adp5587::Driver::KeyPadMappings::D2_ON, Step(KeyState::OFF, NoteSwitchMapping::c0, default_colour, 10,  9,  10)},
    {adp5587::Driver::KeyPadMappings::D3_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 11,  13, 11)},
    {adp5587::Driver::KeyPadMappings::D4_ON, Step(KeyState::OFF, NoteSwitchMapping::c2, default_colour, 12,  14, 12)},
    {adp5587::Driver::KeyPadMappings::D5_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 13,  10, 13)},
    {adp5587::Driver::KeyPadMappings::D6_ON, Step(KeyState::OFF, NoteSwitchMapping::c0, default_colour, 14,  15, 14)},
    {adp5587::Driver::KeyPadMappings::D7_ON, Step(KeyState::OFF, NoteSwitchMapping::c1, default_colour, 15,  11, 15)},       
}};    


SequenceManager::SequenceManager(
    TIM_TypeDef* sequencer_tempo_timer, 
    TIM_TypeDef *sequencer_encoder_timer,
    ssd1306::DriverSerialInterface display_spi_interface, 
    TIM_TypeDef *display_refresh_timer,
    I2C_TypeDef *ad5587_keypad_i2c,
    TIM_TypeDef *ad5587_keypad_debounce_timer,
    I2C_TypeDef *adg2188_control_sw_i2c,
    SPI_TypeDef *tlc5955_led_spi) 
    
    :   m_sequencer_tempo_timer(sequencer_tempo_timer), 
        m_sequencer_encoder_timer(sequencer_encoder_timer),
        m_ssd1306_display_spi(bass_station::DisplayManager(display_spi_interface, display_refresh_timer)),
        m_ad5587_keypad_i2c(bass_station::KeypadManager(ad5587_keypad_i2c, ad5587_keypad_debounce_timer)),
        m_synth_control_switch(adg2188::Driver(adg2188_control_sw_i2c)),
        m_led_manager(bass_station::LedManager(tlc5955_led_spi))
{


    // send configuration data to TLC5955
    m_led_manager.send_control_data();

    // enable the timer and set a sensible starting "tempo"
    LL_TIM_EnableCounter(m_sequencer_encoder_timer.get());
    m_sequencer_encoder_timer->CNT = 128;

    // setup this class as timer callback
    // SequenceManager needs to be enabled first, because ISR has higher priority (0)
    m_sequencer_tempo_timer_isr_handler.initialise(this);
    LL_TIM_EnableCounter(m_sequencer_tempo_timer.get());
	LL_TIM_EnableIT_UPDATE(m_sequencer_tempo_timer.get());

    // Start the display refresh timer interrupts *after* the sequencer tempo timer interrupts
    m_ssd1306_display_spi.start_isr();
}

void SequenceManager::tempo_timer_isr()
{
    // probably needs its own timer callback as this will become less responsive at slower tempos
    update_display_and_tempo();

    // get latest key events from adp5587
    m_ad5587_keypad_i2c.process_key_events(m_sequence_map);

    // update the LED and synth control switch for the next sequence position
    increment_and_execute_sequence_step();

    // reset the UIF bit to re-enable interrupts
    LL_TIM_ClearFlag_UPDATE(m_sequencer_tempo_timer.get());

}

void SequenceManager::update_display_and_tempo()
{
    // update the display with the sequencer position index
    std::string beat_pos{"Position: "};
    beat_pos += std::to_string(m_step_position) + ' ';
    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);
    
    // update the display with the encoder count value
    std::string encoder_pos{"Tempo: "};
    encoder_pos += std::to_string(m_sequencer_encoder_timer->CNT) + "   ";
    m_ssd1306_display_spi.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);

    // update the sequencer tempo (prescaler) with the encoder count value
    // TODO this is backwards: Should be CW = increase tempo, CCW = decrease tempo
    m_sequencer_tempo_timer->PSC = m_sequencer_encoder_timer->CNT;    
}

void SequenceManager::increment_and_execute_sequence_step(bool run_demo_only)
{
    if (run_demo_only)
    {
        // this is kinda broken but it does something colourful
        m_led_manager.update_ladder_demo(m_sequence_map, 0xFFFF, 100);
    }   
    else
    {
        // get the Step object for the current sequence position
        Step &current_step = m_sequence_map.data.at(m_sequencer_key_mapping.at(m_step_position)).second;

        // save the colour and state
        LedColour previous_colour = current_step.m_colour;
        KeyState previous_state = current_step.m_key_state;
        
        

        if (current_step.m_key_state == KeyState::ON)
        {
            // update LED colour
            current_step.m_colour = beat_colour_on;
            

            m_synth_control_switch.write_switch(
                adg2188::Driver::Throw::open, 
                static_cast<adg2188::Driver::Pole>(m_previous_enabled_note),
                adg2188::Driver::Latch::set); 

            m_synth_control_switch.write_switch(
                adg2188::Driver::Throw::close, 
                static_cast<adg2188::Driver::Pole>(current_step.m_note),
                adg2188::Driver::Latch::set);        
            
            // retain the note we enabled this iteration so we can turn it off in the next iteration
            m_previous_enabled_note = current_step.m_note;                
        }
        else
        {
            // update the LED colour
            current_step.m_colour = beat_colour_off;

            m_synth_control_switch.write_switch(
                adg2188::Driver::Throw::open, 
                static_cast<adg2188::Driver::Pole>(m_previous_enabled_note),
                adg2188::Driver::Latch::set); 
        }    



        // turn on the current step in the sequence
        current_step.m_key_state = KeyState::ON;

        // send the entire updated sequence to the TL5955 driver
        m_led_manager.send_both_rows_greyscale_data(m_sequence_map);
        
        // restore the state of the current step (so it is cleared on the next iteration)
        current_step.m_colour = previous_colour;
        current_step.m_key_state = previous_state;

        // increment the current step position
        (m_step_position >= m_sequencer_key_mapping.size() -1) ? m_step_position = 0: m_step_position++;

    }


}

} // namespace bass_station