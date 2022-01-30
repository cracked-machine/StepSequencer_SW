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

    // enable the timer and set a sensible starting "tempo"
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

void SequenceManager::tempo_timer_isr()
{
    // probably needs its own timer callback as this will become less responsive at slower tempos
    update_display_and_tempo();

    // get latest key events from adp5587
    process_key_events();

    // update the LED and synth control switch for the next sequence position
    increment_and_execute_sequence_step();

    // reset the UIF bit to re-enable interrupts
    LL_TIM_ClearFlag_UPDATE(m_tempo_timer.get());

}

void SequenceManager::update_display_and_tempo()
{
    // update the display with the sequencer position index
    std::string beat_pos{"Position: "};
    beat_pos += std::to_string(m_step_position) + ' ';
    m_oled.set_display_line(DisplayManager::DisplayLine::LINE_ONE, beat_pos);
    
    // update the display with the encoder count value
    std::string encoder_pos{"Tempo: "};
    encoder_pos += std::to_string(m_encoder_timer->CNT) + "   ";
    m_oled.set_display_line(DisplayManager::DisplayLine::LINE_TWO, encoder_pos);

    // update the sequencer tempo (prescaler) with the encoder count value
    // TODO this is backwards: Should be CW = increase tempo, CCW = decrease tempo
    m_tempo_timer->PSC = m_encoder_timer->CNT;    
}

void SequenceManager::increment_and_execute_sequence_step(bool run_demo_only)
{
    if (run_demo_only)
    {
        // this is kinda broken but it does something colourful
        m_led_manager.update_ladder_demo(the_sequence, 0xFFFF, 100);
    }   
    else
    {
        // get the Step object for the current sequence position
        Step &current_step = the_sequence.data.at(m_sequencer_key_mapping.at(m_step_position)).second;

        // save the colour and state
        LedColour previous_colour = current_step.m_colour;
        KeyState previous_state = current_step.m_key_state;
        
        // temporariliy update the state of the current step
        current_step.m_key_state = KeyState::ON;

        // check if the step was selected by the user
        if (previous_state == KeyState::ON)
        {
            // update LED colour
            current_step.m_colour = m_beat_colour_on;
            // close synth control switch
            m_synth_control_switch.write_switch(
                adg2188::Driver::Throw::close, 
                adg2188::Driver::Pole::x4_to_y2,
                adg2188::Driver::Latch::set);            
        }
        else
        {
            // update the LED colour
            current_step.m_colour = m_beat_colour_off;
            // open the synth control switch
            m_synth_control_switch.write_switch(
                adg2188::Driver::Throw::open, 
                adg2188::Driver::Pole::x4_to_y2,
                adg2188::Driver::Latch::set);            
        }
        
        // send the entire updated sequence to the TL5955 driver
        m_led_manager.send_both_rows_greyscale_data(the_sequence);
        
        // restore the state of the current step (so it is cleared on the next iteration)
        current_step.m_colour = previous_colour;
        current_step.m_key_state = previous_state;

        // increment the current step position
        (m_step_position >= m_sequencer_key_mapping.size() -1) ? m_step_position = 0: m_step_position++;

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