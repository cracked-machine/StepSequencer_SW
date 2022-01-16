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
    led_manager.send_control_data();
}

void SequenceManager::process_key_events()
{
    // std::array<adp5587::Driver::KeyPadMappings, 10U> key_events_list;
    // keyscanner.get_key_events(key_events_list);
    // for (adp5587::Driver::KeyPadMappings key_event : key_events_list)
    // {
    //     switch (key_event)
    //     {
    //         // if key "A2" was pressed toggle key "A2" state in the step sequence store
    //         case adp5587::Driver::KeyPadMappings::A2_ON:
    //             if (m_step_sequence.at(sequence_order.at(1)).m_state == adp5587::Driver::KeyPadMappings::A2_OFF)
    //                 m_step_sequence.at(sequence_order.at(1)).m_state = adp5587::Driver::KeyPadMappings::A2_ON;
    //             else
    //                 m_step_sequence.at(sequence_order.at(1)).m_state = adp5587::Driver::KeyPadMappings::A2_OFF;
    //             break;
    //         default:
    //             break;
    //     }
    // }    
}

void SequenceManager::execute_sequence(uint16_t delay_ms [[maybe_unused]])
{
    led_manager.clear_all_leds();

    process_key_events();

    // save the previous colour and state for the current beat position key
    LedColour previous_colour = m_step_sequence.at(sequence_order.at(beat_position)).m_colour;
    adp5587::Driver::KeyPadMappings previous_state = m_step_sequence.at(sequence_order.at(beat_position)).m_state;
    
    // change the sequence data for the current beat position key
    m_step_sequence.at(sequence_order.at(beat_position)).m_state = adp5587::Driver::KeyPadMappings::A0_ON;
    if (previous_state >= adp5587::Driver::KeyPadMappings::A0_ON)
    {
        m_step_sequence.at(sequence_order.at(beat_position)).m_colour = LedColour::red;
    }
    else
    {
        m_step_sequence.at(sequence_order.at(beat_position)).m_colour = LedColour::cyan;
    }
    
    
    // apply the sequence with the change
    led_manager.send_both_rows_greyscale_data(m_step_sequence);
    
    // restore the previous colour and state for the current beat position key
    m_step_sequence.at(sequence_order.at(beat_position)).m_colour = previous_colour;
    m_step_sequence.at(sequence_order.at(beat_position)).m_state = previous_state;

    // increment
    (beat_position >= sequence_order.size() -1) ? beat_position = 0: beat_position++;
    LL_mDelay(delay_ms);
}


} // namespace bass_station