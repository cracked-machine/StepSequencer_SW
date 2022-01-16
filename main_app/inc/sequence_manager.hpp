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

#ifndef __SEQUENCE_MANAGER_HPP__
#define __SEQUENCE_MANAGER_HPP__

#include <array>

#include <step.hpp>
#include <led_manager.hpp>


namespace bass_station
{

class SequenceManager
{
public: 

    SequenceManager();
    void execute_sequence(uint16_t delay_ms);

private:

    std::array<Step, 32> m_step_sequence
    {{
        Step(0, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A0_ON, LedColour::yellow),
        Step(1, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A1_OFF, LedColour::yellow),
        Step(2, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A2_OFF, LedColour::yellow),
        Step(3, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A3_OFF, LedColour::yellow),
        Step(4, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A4_ON, LedColour::yellow),
        Step(5, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A5_OFF, LedColour::yellow),
        Step(6, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A6_OFF, LedColour::yellow),
        Step(7, SequencerRow::lower, adp5587::Driver::KeyPadMappings::A7_OFF, LedColour::yellow),

        Step(8, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B0_ON, LedColour::yellow),
        Step(9, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B1_OFF, LedColour::yellow),
        Step(10, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B2_OFF, LedColour::yellow),
        Step(11, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B3_OFF, LedColour::yellow),
        Step(12, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B4_ON, LedColour::yellow),
        Step(13, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B5_OFF, LedColour::yellow),
        Step(14, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B6_OFF, LedColour::yellow),
        Step(15, SequencerRow::lower, adp5587::Driver::KeyPadMappings::B7_OFF, LedColour::yellow),

        Step(0, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C0_ON, LedColour::yellow),
        Step(1, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C1_OFF, LedColour::yellow),
        Step(2, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C2_OFF, LedColour::yellow),
        Step(3, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C3_OFF, LedColour::yellow),
        Step(4, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C4_ON, LedColour::yellow),
        Step(5, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C5_OFF, LedColour::yellow),
        Step(6, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C6_OFF, LedColour::yellow),
        Step(7, SequencerRow::upper, adp5587::Driver::KeyPadMappings::C7_OFF, LedColour::yellow),

        Step(8, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D0_ON, LedColour::yellow),
        Step(9, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D1_OFF, LedColour::yellow),
        Step(10, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D2_OFF, LedColour::yellow),
        Step(11, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D3_OFF, LedColour::yellow),
        Step(12, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D4_ON, LedColour::yellow),
        Step(13, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D5_OFF, LedColour::yellow),
        Step(14, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D6_OFF, LedColour::yellow),
        Step(15, SequencerRow::upper, adp5587::Driver::KeyPadMappings::D7_OFF, LedColour::yellow),     
    }};

    bass_station::LedManager led_manager {SPI2};
    adp5587::Driver keyscanner {I2C3};

    uint8_t beat_position {0};
    std::array<uint8_t, 32> sequence_order {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    void process_key_events();
};

}   // namespace bass_station


#endif // __SEQUENCE_MANAGER_HPP__