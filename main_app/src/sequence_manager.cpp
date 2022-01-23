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

#ifdef USE_STATIC_MAP_TYPE
void SequenceManager::execute_sequence_map(uint16_t delay_ms [[maybe_unused]])
{
    process_key_events_map();

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
    m_led_manager.send_both_rows_greyscale_data(the_sequence.data);
    
    // restore the previous colour and state for the current beat position key
    current_step.m_colour = previous_colour;
    current_step.m_key_state = previous_state;

    // increment
    (m_beat_position >= m_sequencer_key_mapping.size() -1) ? m_beat_position = 0: m_beat_position++;
    LL_mDelay(delay_ms);  
}

void SequenceManager::process_key_events_map()
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

#else // (not defined) USE_STATIC_MAP_TYPE

void SequenceManager::execute_sequence(uint16_t delay_ms [[maybe_unused]])
{
    // m_led_manager.clear_all_leds();
    
    process_key_events();

    // save the previous colour and state for the current beat position key
    LedColour previous_colour = m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_colour;
    adp5587::Driver::KeyPadMappings previous_state = m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_state;
    
    // change the sequence data for the current beat position key
    m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_state = adp5587::Driver::KeyPadMappings::A0_ON;
    if (previous_state >= adp5587::Driver::KeyPadMappings::A0_ON)
    {
        m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_colour = m_beat_colour_on;
    }
    else
    {
        m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_colour = m_beat_colour_off;
    }
    
    
    // apply the sequence with the change
    m_led_manager.send_both_rows_greyscale_data(m_step_sequence);
    
    // restore the previous colour and state for the current beat position key
    m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_colour = previous_colour;
    m_step_sequence.at(m_sequencer_key_mapping.at(m_beat_position)).m_state = previous_state;

    // increment
    (m_beat_position >= m_sequencer_key_mapping.size() -1) ? m_beat_position = 0: m_beat_position++;
    LL_mDelay(delay_ms);
}


void SequenceManager::process_key_events()
{
    // get the key events FIFO list from the ADP5587 driver 
    std::array<adp5587::Driver::KeyPadMappings, 10U> key_events_list;
    m_keyscanner.get_key_events(key_events_list);

    // check if any key presses matched the 3rd key, lower row
    for (adp5587::Driver::KeyPadMappings key_event : key_events_list)
    {
        switch (key_event)
        {
            // lower row keys 0-7
            case adp5587::Driver::KeyPadMappings::A0_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A0).m_state == adp5587::Driver::KeyPadMappings::A0_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A0).m_state = adp5587::Driver::KeyPadMappings::A0_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A0).m_state = adp5587::Driver::KeyPadMappings::A0_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::A1_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A1).m_state == adp5587::Driver::KeyPadMappings::A1_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A1).m_state = adp5587::Driver::KeyPadMappings::A1_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A1).m_state = adp5587::Driver::KeyPadMappings::A1_OFF;
                break;                            
            case adp5587::Driver::KeyPadMappings::A2_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A2).m_state == adp5587::Driver::KeyPadMappings::A2_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A2).m_state = adp5587::Driver::KeyPadMappings::A2_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A2).m_state = adp5587::Driver::KeyPadMappings::A2_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::A3_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A3).m_state == adp5587::Driver::KeyPadMappings::A3_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A3).m_state = adp5587::Driver::KeyPadMappings::A3_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A3).m_state = adp5587::Driver::KeyPadMappings::A3_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::A4_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A4).m_state == adp5587::Driver::KeyPadMappings::A4_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A4).m_state = adp5587::Driver::KeyPadMappings::A4_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A4).m_state = adp5587::Driver::KeyPadMappings::A4_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::A5_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A5).m_state == adp5587::Driver::KeyPadMappings::A5_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A5).m_state = adp5587::Driver::KeyPadMappings::A5_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A5).m_state = adp5587::Driver::KeyPadMappings::A5_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::A6_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A6).m_state == adp5587::Driver::KeyPadMappings::A6_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A6).m_state = adp5587::Driver::KeyPadMappings::A6_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A6).m_state = adp5587::Driver::KeyPadMappings::A6_OFF;
                break;       
            case adp5587::Driver::KeyPadMappings::A7_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::A7).m_state == adp5587::Driver::KeyPadMappings::A7_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::A7).m_state = adp5587::Driver::KeyPadMappings::A7_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::A7).m_state = adp5587::Driver::KeyPadMappings::A7_OFF;
                break;                                                                         
  
            // lower row keys 8-15
            case adp5587::Driver::KeyPadMappings::B0_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B0).m_state == adp5587::Driver::KeyPadMappings::B0_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B0).m_state = adp5587::Driver::KeyPadMappings::B0_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B0).m_state = adp5587::Driver::KeyPadMappings::B0_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::B1_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B1).m_state == adp5587::Driver::KeyPadMappings::B1_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B1).m_state = adp5587::Driver::KeyPadMappings::B1_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B1).m_state = adp5587::Driver::KeyPadMappings::B1_OFF;
                break;                            
            case adp5587::Driver::KeyPadMappings::B2_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B2).m_state == adp5587::Driver::KeyPadMappings::B2_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B2).m_state = adp5587::Driver::KeyPadMappings::B2_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B2).m_state = adp5587::Driver::KeyPadMappings::B2_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::B3_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B3).m_state == adp5587::Driver::KeyPadMappings::B3_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B3).m_state = adp5587::Driver::KeyPadMappings::B3_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B3).m_state = adp5587::Driver::KeyPadMappings::B3_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::B4_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B4).m_state == adp5587::Driver::KeyPadMappings::B4_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B4).m_state = adp5587::Driver::KeyPadMappings::B4_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B4).m_state = adp5587::Driver::KeyPadMappings::B4_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::B5_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B5).m_state == adp5587::Driver::KeyPadMappings::B5_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B5).m_state = adp5587::Driver::KeyPadMappings::B5_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B5).m_state = adp5587::Driver::KeyPadMappings::B5_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::B6_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B6).m_state == adp5587::Driver::KeyPadMappings::B6_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B6).m_state = adp5587::Driver::KeyPadMappings::B6_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B6).m_state = adp5587::Driver::KeyPadMappings::B6_OFF;
                break;       
            case adp5587::Driver::KeyPadMappings::B7_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::B7).m_state == adp5587::Driver::KeyPadMappings::B7_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::B7).m_state = adp5587::Driver::KeyPadMappings::B7_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::B7).m_state = adp5587::Driver::KeyPadMappings::B7_OFF;
                break;  

            // upper row keys 0-7
            case adp5587::Driver::KeyPadMappings::C0_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C0).m_state == adp5587::Driver::KeyPadMappings::C0_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C0).m_state = adp5587::Driver::KeyPadMappings::C0_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C0).m_state = adp5587::Driver::KeyPadMappings::C0_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::C1_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C1).m_state == adp5587::Driver::KeyPadMappings::C1_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C1).m_state = adp5587::Driver::KeyPadMappings::C1_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C1).m_state = adp5587::Driver::KeyPadMappings::C1_OFF;
                break;                            
            case adp5587::Driver::KeyPadMappings::C2_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C2).m_state == adp5587::Driver::KeyPadMappings::C2_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C2).m_state = adp5587::Driver::KeyPadMappings::C2_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C2).m_state = adp5587::Driver::KeyPadMappings::C2_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::C3_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C3).m_state == adp5587::Driver::KeyPadMappings::C3_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C3).m_state = adp5587::Driver::KeyPadMappings::C3_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C3).m_state = adp5587::Driver::KeyPadMappings::C3_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::C4_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C4).m_state == adp5587::Driver::KeyPadMappings::C4_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C4).m_state = adp5587::Driver::KeyPadMappings::C4_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C4).m_state = adp5587::Driver::KeyPadMappings::C4_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::C5_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C5).m_state == adp5587::Driver::KeyPadMappings::C5_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C5).m_state = adp5587::Driver::KeyPadMappings::C5_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C5).m_state = adp5587::Driver::KeyPadMappings::C5_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::C6_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C6).m_state == adp5587::Driver::KeyPadMappings::C6_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C6).m_state = adp5587::Driver::KeyPadMappings::C6_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C6).m_state = adp5587::Driver::KeyPadMappings::C6_OFF;
                break;       
            case adp5587::Driver::KeyPadMappings::C7_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::C7).m_state == adp5587::Driver::KeyPadMappings::C7_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::C7).m_state = adp5587::Driver::KeyPadMappings::C7_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::C7).m_state = adp5587::Driver::KeyPadMappings::C7_OFF;
                break;  

            // upper row keys 8-15
            case adp5587::Driver::KeyPadMappings::D0_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D0).m_state == adp5587::Driver::KeyPadMappings::D0_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D0).m_state = adp5587::Driver::KeyPadMappings::D0_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D0).m_state = adp5587::Driver::KeyPadMappings::D0_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::D1_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D1).m_state == adp5587::Driver::KeyPadMappings::D1_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D1).m_state = adp5587::Driver::KeyPadMappings::D1_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D1).m_state = adp5587::Driver::KeyPadMappings::D1_OFF;
                break;                            
            case adp5587::Driver::KeyPadMappings::D2_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D2).m_state == adp5587::Driver::KeyPadMappings::D2_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D2).m_state = adp5587::Driver::KeyPadMappings::D2_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D2).m_state = adp5587::Driver::KeyPadMappings::D2_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::D3_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D3).m_state == adp5587::Driver::KeyPadMappings::D3_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D3).m_state = adp5587::Driver::KeyPadMappings::D3_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D3).m_state = adp5587::Driver::KeyPadMappings::D3_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::D4_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D4).m_state == adp5587::Driver::KeyPadMappings::D4_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D4).m_state = adp5587::Driver::KeyPadMappings::D4_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D4).m_state = adp5587::Driver::KeyPadMappings::D4_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::D5_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D5).m_state == adp5587::Driver::KeyPadMappings::D5_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D5).m_state = adp5587::Driver::KeyPadMappings::D5_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D5).m_state = adp5587::Driver::KeyPadMappings::D5_OFF;
                break;
            case adp5587::Driver::KeyPadMappings::D6_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D6).m_state == adp5587::Driver::KeyPadMappings::D6_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D6).m_state = adp5587::Driver::KeyPadMappings::D6_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D6).m_state = adp5587::Driver::KeyPadMappings::D6_OFF;
                break;       
            case adp5587::Driver::KeyPadMappings::D7_ON:
                if (m_step_sequence.at(LogicalKeyArrayMapping::D7).m_state == adp5587::Driver::KeyPadMappings::D7_OFF)
                    m_step_sequence.at(LogicalKeyArrayMapping::D7).m_state = adp5587::Driver::KeyPadMappings::D7_ON;
                else
                    m_step_sequence.at(LogicalKeyArrayMapping::D7).m_state = adp5587::Driver::KeyPadMappings::D7_OFF;
                break;  
            default:
                break;                
        }
    }    
}

#endif // USE_STATIC_MAP_TYPE




} // namespace bass_station