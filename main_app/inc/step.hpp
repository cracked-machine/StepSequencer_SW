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

#ifndef __STEP_HPP__
#define __STEP_HPP__

#include <cstdint>
#include <adp5587.hpp>

namespace bass_station
{

#ifndef USE_STATIC_MAP_TYPE
enum LogicalKeyArrayMapping
{
    A7=7,	A6=6,	A5=5,	A4=4,	A3=3,	A2=2,	A1=1,	A0=0,
    B7=15,	B6=14,	B5=13,	B4=12,	B3=11,	B2=10,	B1=9,	B0=8,
    C7=23,	C6=22,	C5=21,	C4=20,	C3=19,	C2=18,	C1=17,	C0=16,
    D7=31,	D6=30,	D5=29,	D4=28,	D3=27,	D2=26,	D1=25,	D0=24,
};
#endif
// @brief Refers to the two TLC5955 chips in the bass station sequencer PCB
enum class SequencerRow {
    upper,
    lower
};    

// @brief Basic state for keys
enum class KeyState {
    ON,
    OFF,
};    


// @brief The preset colours available
enum class LedColour {
    red,
    blue,
    green,
    magenta,
    yellow,
    cyan,
    white,
};

#ifdef USE_STATIC_MAP_TYPE
struct Step
{

    Step(KeyState key_state, LedColour colour)
    : m_key_state(key_state), m_colour(colour)
    {
        // nothing else to do here
    }
    KeyState m_key_state;
    LedColour m_colour;
    
};
#else // (not defined) USE_STATIC_MAP_TYPE
struct Step
{
    Step(KeyState key_state, uint8_t position, SequencerRow row, adp5587::Driver::KeyPadMappings state, LedColour colour)
    : m_key_state(key_state), m_position(position), m_row(row), m_state(state), m_colour(colour)
    {
        // nothing else to do here
    }
    KeyState m_key_state;
    const uint8_t m_position;
    const SequencerRow m_row;
    adp5587::Driver::KeyPadMappings m_state;
    LedColour m_colour;
    
};
#endif // USE_STATIC_MAP_TYPE


} // namespace bass_station

#endif // __STEP_HPP__