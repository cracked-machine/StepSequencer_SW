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
#include <static_map.hpp>

// The ADP5587 HW maps the following values to the keys press/release events on the bass station sequencer
//              1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16
//  UpperRow    131/3   141/13  151/23  161/33  171/43  181/53  191/63  201/73  132/4   142/14  152/24  162/34  172/44  182/54  192/64  202/74
//  LowerRow    129/1   139/11  149/21  159/31  169/41  179/51  189/61  199/71  130/2   140/12  150/22  160/32  170/42  180/52  190/62  200/72

namespace bass_station
{



constexpr LedColour m_default_colour { LedColour::white };
constexpr LedColour m_beat_colour_off { LedColour::white };
constexpr LedColour m_beat_colour_on { LedColour::blue };

// The sequence data
static std::array< std::pair< adp5587::Driver::KeyPadMappings, Step >, 32 > key_data{
{
    {adp5587::Driver::KeyPadMappings::A0_ON, Step(KeyState::ON,  m_default_colour, 0)},
    {adp5587::Driver::KeyPadMappings::A1_ON, Step(KeyState::OFF, m_default_colour, 1)},
    {adp5587::Driver::KeyPadMappings::A2_ON, Step(KeyState::OFF, m_default_colour, 2)},
    {adp5587::Driver::KeyPadMappings::A3_ON, Step(KeyState::OFF, m_default_colour, 3)},
    {adp5587::Driver::KeyPadMappings::A4_ON, Step(KeyState::ON,  m_default_colour, 4)},
    {adp5587::Driver::KeyPadMappings::A5_ON, Step(KeyState::OFF, m_default_colour, 5)},
    {adp5587::Driver::KeyPadMappings::A6_ON, Step(KeyState::OFF, m_default_colour, 6)},
    {adp5587::Driver::KeyPadMappings::A7_ON, Step(KeyState::OFF, m_default_colour, 7)},

    {adp5587::Driver::KeyPadMappings::B0_ON, Step(KeyState::ON,  m_default_colour, 8)},
    {adp5587::Driver::KeyPadMappings::B1_ON, Step(KeyState::OFF, m_default_colour, 9)},
    {adp5587::Driver::KeyPadMappings::B2_ON, Step(KeyState::OFF, m_default_colour, 10)},
    {adp5587::Driver::KeyPadMappings::B3_ON, Step(KeyState::OFF, m_default_colour, 11)},
    {adp5587::Driver::KeyPadMappings::B4_ON, Step(KeyState::ON,  m_default_colour, 12)},
    {adp5587::Driver::KeyPadMappings::B5_ON, Step(KeyState::OFF, m_default_colour, 13)},
    {adp5587::Driver::KeyPadMappings::B6_ON, Step(KeyState::OFF, m_default_colour, 14)},
    {adp5587::Driver::KeyPadMappings::B7_ON, Step(KeyState::OFF, m_default_colour, 15)}, 

    {adp5587::Driver::KeyPadMappings::C0_ON, Step(KeyState::ON,  m_default_colour, 0)},
    {adp5587::Driver::KeyPadMappings::C1_ON, Step(KeyState::OFF, m_default_colour, 1)},
    {adp5587::Driver::KeyPadMappings::C2_ON, Step(KeyState::OFF, m_default_colour, 2)},
    {adp5587::Driver::KeyPadMappings::C3_ON, Step(KeyState::OFF, m_default_colour, 3)},
    {adp5587::Driver::KeyPadMappings::C4_ON, Step(KeyState::ON,  m_default_colour, 4)},
    {adp5587::Driver::KeyPadMappings::C5_ON, Step(KeyState::OFF, m_default_colour, 5)},
    {adp5587::Driver::KeyPadMappings::C6_ON, Step(KeyState::OFF, m_default_colour, 6)},
    {adp5587::Driver::KeyPadMappings::C7_ON, Step(KeyState::OFF, m_default_colour, 7)},  

    {adp5587::Driver::KeyPadMappings::D0_ON, Step(KeyState::ON,  m_default_colour, 8)},
    {adp5587::Driver::KeyPadMappings::D1_ON, Step(KeyState::OFF, m_default_colour, 9)},
    {adp5587::Driver::KeyPadMappings::D2_ON, Step(KeyState::OFF, m_default_colour, 10)},
    {adp5587::Driver::KeyPadMappings::D3_ON, Step(KeyState::OFF, m_default_colour, 11)},
    {adp5587::Driver::KeyPadMappings::D4_ON, Step(KeyState::ON,  m_default_colour, 12)},
    {adp5587::Driver::KeyPadMappings::D5_ON, Step(KeyState::OFF, m_default_colour, 13)},
    {adp5587::Driver::KeyPadMappings::D6_ON, Step(KeyState::OFF, m_default_colour, 14)},
    {adp5587::Driver::KeyPadMappings::D7_ON, Step(KeyState::OFF, m_default_colour, 15)},       
    
}};


class SequenceManager
{
public:
    SequenceManager();
private:

    bass_station::LedManager m_led_manager {SPI2};
    adp5587::Driver m_keyscanner {I2C3};

    uint8_t m_beat_position {0};

    // @brief This determines the positional order in which the cursor sweeps the sequence
    // This begins on the upper row and ends on the lower row, sweeping left to right
    std::array<uint8_t, 32> m_sequencer_key_mapping {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


public: 
    void execute_sequence(uint16_t delay_ms);
private:
    void process_key_events();

    // @brief The map initialised with sequence data
    noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, key_data.size()> the_sequence = 
        noarch::containers::StaticMap<adp5587::Driver::KeyPadMappings, Step, key_data.size()>{{key_data}};

};


}   // namespace bass_station


#endif // __SEQUENCE_MANAGER_HPP__