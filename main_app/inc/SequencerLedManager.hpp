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

#ifndef __SEQUENCERLEDMANAGER_HPP__
#define __SEQUENCERLEDMANAGER_HPP__

#include <tlc5955.hpp>

namespace bass_station 
{

// @brief manages the tlc5955 driver
class SequencerLedManager
{
public:
    // @brief The preset colours available
    enum class LedColour {
        red,
        blue,
        green
    };

    // @brief The two rows in the bass station sequencer PCB
    enum class SequencerRow {
        upper,
        lower
    };    

    // @brief toggle the LAT pin after each full write to the chip common register
    enum class LatchOption {
        enable,
        disable
    };

    // @brief Construct a new Sequencer Led Manager object
    SequencerLedManager() = default;

    // @brief write control data to buffer and send via SPI
    void send_control_data();

    // @brief Write greyscale (GS) data to the buffer and send via SPI
    // @param led_position index position within row: 0-15
    // @param row The sequencer row: SequencerRow::upper or SequencerRow::lower
    // @param greyscale_pwm Constrast of LED: 0-65535
    // @param colour Preset colour: LedColour
    // @param latch_option LatchOption
    void send_greyscale_data(uint16_t led_position, const SequencerRow &row, uint16_t greyscale_pwm, const LedColour &colour, const LatchOption &latch_option);
    
    // @brief Run a simple demo that runs boths rows 0->15 then 15->0, for red, green and blue.
    // @param pwm_value The constrast for the iteration
    // @param delay_ms The delay between each iteration. Affects the speed of the demo
    void update_ladder_demo(uint16_t pwm_value, uint32_t delay_ms);    


private:
    // @brief The TLC5955 driver instance
    tlc5955::Driver tlc5955_driver;

    // @brief lower row mapping of indices to the physical wiring to the TLC5955 chip on the PCB
    std::array<uint16_t, 16> lower_row_led_mapping = {4,0,5,1,2,6,3,7, 11,15,10,14,13,9,12,8};

    // @brief upper row mapping of indices to the physical wiring to the TLC5955 chip on the PCB
    std::array<uint16_t, 16> upper_row_led_mapping = {7,3,6,2,1,5,0,4, 8,12,9,13,14,10,15,11};
};

} // namespace bass_station

#endif // __SEQUENCERLEDMANAGER_HPP__