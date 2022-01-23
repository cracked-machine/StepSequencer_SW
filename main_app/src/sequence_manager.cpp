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

SequenceManager::SequenceManager()
{
    m_led_manager.send_control_data();
}

void SequenceManager::execute_sequence(uint16_t delay_ms [[maybe_unused]], bool run_demo_only)
{
    if (run_demo_only)
    {
        m_led_manager.update_ladder_demo(the_sequence, 0xFFFF, delay_ms);
    }   
    else
    {
        process_key_events();

        Step &current_step = the_sequence.data.at(m_sequencer_key_mapping.at(m_beat_position)).second;

        // save the previous colour and state for the current beat position key
        LedColour previous_colour = current_step.m_colour;
        KeyState previous_state = current_step.m_key_state;
        
        // change the sequence data for the current beat position key
        current_step.m_key_state = KeyState::ON;
        if (previous_state == KeyState::ON)
        {
            current_step.m_colour = m_beat_colour_on;
        }
        else
        {
            current_step.m_colour = m_beat_colour_off;
        }
        
        // apply the sequence with the change
        m_led_manager.send_both_rows_greyscale_data(the_sequence);
        
        // restore the previous colour and state for the current beat position key
        current_step.m_colour = previous_colour;
        current_step.m_key_state = previous_state;

        // increment
        (m_beat_position >= m_sequencer_key_mapping.size() -1) ? m_beat_position = 0: m_beat_position++;
        
        LL_mDelay(delay_ms);  
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