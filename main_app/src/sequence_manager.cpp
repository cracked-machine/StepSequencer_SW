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

SequenceManager::SequenceManager(TIM_TypeDef* tempo_timer, TIM_TypeDef *encoder_timer) 
    : m_tempo_timer(tempo_timer), m_encoder_timer(encoder_timer)
{
    // send configuration data to TLC5955
    m_led_manager.send_control_data();

    LL_TIM_EnableCounter(m_encoder_timer.get());
    m_encoder_timer->CNT = 128;

    // setup this class as timer callback
    // SequenceManager needs to be enabled first, because ISR has higher priority (0)
    m_tempo_timer_isr_handler.initialise(this);
    LL_TIM_EnableCounter(m_tempo_timer.get());
	LL_TIM_EnableIT_UPDATE(m_tempo_timer.get());

    // DisplayManager needs to be enabled second
    m_oled.start_isr();
}

void SequenceManager::update_display_and_tempo()
{
    // update the display with the encoder count value
    std::string encoder_pos{"Tempo: "};
    encoder_pos += std::to_string(m_encoder_timer->CNT) + "   ";
    m_oled.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);

    // update the sequencer tempo (prescaler) with the encoder count value
    
    m_tempo_timer->PSC = m_encoder_timer->CNT;    
}

void SequenceManager::tempo_timer_isr()
{
    update_display_and_tempo();
    execute_sequence();
    LL_TIM_ClearFlag_UPDATE(m_tempo_timer.get());

}

void SequenceManager::execute_sequence(bool run_demo_only)
{
    if (run_demo_only)
    {
        m_led_manager.update_ladder_demo(the_sequence, 0xFFFF, 100);
    }   
    else
    {
        // get latest key events from adp5587
        process_key_events();

        std::string beat_pos{"Position: "};
        beat_pos += std::to_string(m_beat_position) + ' ';
        m_oled.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);

        // get the current step remapped for the sequencer execution order
        Step &current_step = the_sequence.data.at(m_sequencer_key_mapping.at(m_beat_position)).second;

        // save the previous colour and state for the current beat position key
        LedColour previous_colour = current_step.m_colour;
        KeyState previous_state = current_step.m_key_state;
        
        // change the sequence data for the current beat position key
        current_step.m_key_state = KeyState::ON;
        if (previous_state == KeyState::ON)
        {
            // turn the note switch on
            current_step.m_colour = m_beat_colour_on;
            
            xpoint.write_switch(
                adg2188::Driver::Throw::close, 
                adg2188::Driver::Pole::x4_to_y2,
                adg2188::Driver::Latch::set);            
        }
        else
        {
            // turn the note switch off
            current_step.m_colour = m_beat_colour_off;
        
            xpoint.write_switch(
                adg2188::Driver::Throw::open, 
                adg2188::Driver::Pole::x4_to_y2,
                adg2188::Driver::Latch::set);            
        }
        
        // apply the sequence with the change
        m_led_manager.send_both_rows_greyscale_data(the_sequence);
        
        // restore the previous colour and state for the current beat position key
        current_step.m_colour = previous_colour;
        current_step.m_key_state = previous_state;

        // increment
        (m_beat_position >= m_sequencer_key_mapping.size() -1) ? m_beat_position = 0: m_beat_position++;

    }
}

void SequenceManager::process_key_events()
{
    // get the key events FIFO list from the ADP5587 driver 
    std::array<adp5587::Driver::KeyPadMappings, 10U> key_events_list;
    m_keyscanner.get_key_events(key_events_list);
    
    for (adp5587::Driver::KeyPadMappings key_event : key_events_list)
    {
        Step *current_step = the_sequence.find_key(key_event);
        if(current_step == nullptr) { /* no match found in map */ }
        else
        {
            if (current_step->m_key_state == KeyState::OFF)
            {
                current_step->m_key_state = KeyState::ON;
            }
            else
            {
                current_step->m_key_state = KeyState::OFF;
            }
        }
    }
}

} // namespace bass_station