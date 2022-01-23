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

#ifndef __LEDMANAGER_HPP__
#define __LEDMANAGER_HPP__

#include <tlc5955.hpp>
#include <step.hpp>

namespace bass_station 
{

// @brief manages the tlc5955 driver
class LedManager
{
public:

    // @brief toggle the LAT pin after each full write to the chip common register
    enum class LatchOption {
        enable,
        disable
    };

    // @brief Construct a new Sequencer Led Manager object
    LedManager(SPI_TypeDef *spi_handle);

    // @brief write control data to buffer and send via SPI
    // This is sent twice (once for each chip), only latching on the second time
    void send_control_data();

    // @brief Sets one LED only and send the data via SPI
    // @param led_position index position within row: 0-15
    // @param row The sequencer row: SequencerRow::upper or SequencerRow::lower
    // @param greyscale_pwm Constrast of LED: 0-65535
    // @param colour Preset colour: LedColour
    // @param latch_option LatchOption
    void send_one_led_greyscale_data_at(uint16_t led_position, const SequencerRow &row, uint16_t greyscale_pwm, const LedColour &colour, const LatchOption &latch_option);

    // @brief Used to display the selected sequencer keys all in one shot
    // @tparam LED_NUMBER The size of the sequence array (always 32 in this version)
    // @param step_sequence The array of step objects that make up the full sequence
    template <std::size_t LED_NUMBER>
    void send_both_rows_greyscale_data(noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, LED_NUMBER> &step_sequence);
    
    // @brief Run a simple demo that runs boths rows 0->15 then 15->0, for red, green and blue.
    // @param pwm_value The constrast for the iteration
    // @param delay_ms The delay between each iteration. Affects the speed of the demo
    template <std::size_t LED_NUMBER>
    void update_ladder_demo( 
        noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, LED_NUMBER> &sequence_map, 
        uint16_t pwm_value, 
        uint32_t delay_ms);   

    // @brief Writes empty data to the keys. Not as useful as you might think. Will probably cause flickering.
    void clear_all_leds();

    // @brief Convenience function to set the all key leds on to a single colour
    // @param greyscale_pwm 
    // @param colour 
    void set_all_leds(uint16_t greyscale_pwm, const LedColour &colour);

private:
    // @brief The TLC5955 driver instance
    tlc5955::Driver tlc5955_driver;

    // @brief Helper function that maps RGB pwm values to preset primary and secondary colours
    // @param position Set the LED at this position in the buffer
    // @param colour The colour to set it to
    void set_position_and_colour(uint16_t position, LedColour colour);
};

template <std::size_t LED_NUMBER>
void LedManager::send_both_rows_greyscale_data( noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, LED_NUMBER> &sequence_map)
{
    // get the start, mid, end iterators for this input map
    auto start_pos = sequence_map.data.begin();
    auto mid_pos = sequence_map.data.begin() + sequence_map.data.size() / 2;
    auto end_pos = sequence_map.data.end();

	// refresh buffers
	tlc5955_driver.reset();

    // set the TLC5955 register data for the upper row keys
    std::for_each(mid_pos, end_pos, [this, &sequence_map]
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            Step current_step = data_pair.second;
            if (current_step.m_key_state == KeyState::ON)
            {
                // remap the logical array positions to the physical PCB wiring
                set_position_and_colour(current_step.m_physical_mapping_index, current_step.m_colour);             
            }
        }
    );

    // send the upper row data without latch
 	tlc5955_driver.process_register();
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);
    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::no_latch); 

    // clear buffer so that the upper row data that was just sent, does not contaminatate the lower row data we are about to send
	tlc5955_driver.reset();

    // set the TLC5955 register data for the lower row keys
    std::for_each(start_pos, mid_pos, [this, &sequence_map] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            Step current_step = data_pair.second;
            if (current_step.m_key_state == KeyState::ON)
            {
                // remap the logical array positions to the physical PCB wiring
                set_position_and_colour(current_step.m_physical_mapping_index, current_step.m_colour);             
            }
        }
    );    

    // send the lower row data with latch
	tlc5955_driver.process_register();
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);
    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);    
}


template <std::size_t LED_NUMBER>
void LedManager::update_ladder_demo( 
    noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, LED_NUMBER> &sequence_map, 
    uint16_t pwm_value, 
    uint32_t delay_ms)
{


    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::red, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::red, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::yellow, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::yellow, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::green, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::green, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );    

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::cyan, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::cyan, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::blue, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::blue, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );    

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::green, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::green, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );    

    std::for_each(sequence_map.data.begin(), sequence_map.data.end(), [=, this] 
        (std::pair< adp5587::Driver::KeyPadMappings, Step > &data_pair)
        {
            // set the LED colours and positions
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::upper, pwm_value, LedColour::white, LatchOption::disable);
            send_one_led_greyscale_data_at(
                data_pair.second.m_physical_mapping_index, SequencerRow::lower, pwm_value, LedColour::white, LatchOption::enable);
            LL_mDelay(delay_ms);
        }
    );    

}
} // namespace bass_station

#endif // __LEDMANAGER_HPP__