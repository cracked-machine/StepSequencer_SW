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
#include <adg2188.hpp>
#include <static_map.hpp>

namespace bass_station
{


// @brief Refers to the two TLC5955 chips in the bass station sequencer PCB
enum class SequencerRow {
    upper,
    lower
};    

// @brief Basic state for keys
// Not enum class so it can be used as boolean
enum KeyState {
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

constexpr LedColour default_colour { LedColour::white };
constexpr LedColour beat_colour_off { LedColour::white };
constexpr LedColour beat_colour_on { LedColour::blue };

// @brief Maps the musical note to crosspoint switch pole
enum class NoteSwitchMapping
{
    c0          =   static_cast<int>(adg2188::Driver::Pole::x4_to_y0),
    c0_sharp     =   static_cast<int>(adg2188::Driver::Pole::x5_to_y0),
    d0          =   static_cast<int>(adg2188::Driver::Pole::x6_to_y0),
    d0_sharp     =   static_cast<int>(adg2188::Driver::Pole::x7_to_y0),
    e0          =   static_cast<int>(adg2188::Driver::Pole::x0_to_y2),
    f0          =   static_cast<int>(adg2188::Driver::Pole::x1_to_y2),
    f0_sharp     =   static_cast<int>(adg2188::Driver::Pole::x2_to_y2),
    g0          =   static_cast<int>(adg2188::Driver::Pole::x3_to_y2),
    g0_sharp     =   static_cast<int>(adg2188::Driver::Pole::x4_to_y2),
    a1          =   static_cast<int>(adg2188::Driver::Pole::x5_to_y2),
    a1_sharp     =   static_cast<int>(adg2188::Driver::Pole::x6_to_y2),
    b1          =   static_cast<int>(adg2188::Driver::Pole::x7_to_y2),
    c1          =   static_cast<int>(adg2188::Driver::Pole::x0_to_y4),  // Middle C
    c1_sharp     =   static_cast<int>(adg2188::Driver::Pole::x1_to_y4),
    d1          =   static_cast<int>(adg2188::Driver::Pole::x2_to_y4),
    d1_sharp     =   static_cast<int>(adg2188::Driver::Pole::x3_to_y4),
    e1          =   static_cast<int>(adg2188::Driver::Pole::x4_to_y4),
    f1          =   static_cast<int>(adg2188::Driver::Pole::x5_to_y4),
    f1_sharp     =   static_cast<int>(adg2188::Driver::Pole::x6_to_y4),
    g1          =   static_cast<int>(adg2188::Driver::Pole::x7_to_y4),
    g1_sharp     =   static_cast<int>(adg2188::Driver::Pole::x0_to_y6),
    a2          =   static_cast<int>(adg2188::Driver::Pole::x1_to_y6),
    a2_sharp     =   static_cast<int>(adg2188::Driver::Pole::x2_to_y6),
    b2          =   static_cast<int>(adg2188::Driver::Pole::x3_to_y6),
    c2          =   static_cast<int>(adg2188::Driver::Pole::x4_to_y6),
    none        = 0,
};

// @brief Represents a single step/key/button in the sequencer
struct Step
{

    // @brief Construct a new Step object
    // @param key_state ON or OFF
    // @param colour The colour when ON
    // @param logical_mapping_index Maps this step to a position index in one of two 16 position arrays.
    // @param physical_mapping_index Maps this step to the physical wiring pin index of the TLC5955 chip on the PCB
    // @param sequence_mapping_index Maps this step to a position index in the sequence execution order.
    Step(   KeyState key_state,
            NoteSwitchMapping note, 
            LedColour colour, 
            int logical_mapping_index, 
            int physical_mapping_index,
            int sequence_mapping_index)

    :   m_key_state(key_state), 
        m_note(note),
        m_colour(colour), 
        m_logical_mapping_index(logical_mapping_index),
        m_physical_mapping_index(physical_mapping_index),
        m_sequence_mapping_index(sequence_mapping_index)
    {
        // nothing else to do here
    }

    // @brief Is the Key ON or OFF
    KeyState m_key_state;

    NoteSwitchMapping m_note;
    
    // @brief The colour of the key when it is ON
    LedColour m_colour;
    
    // @brief Maps this step to a position index in one of two 16 position arrays.
    int m_logical_mapping_index;

    // @brief Maps this step to the physical wiring pin index of the TLC5955 chip on the PCB
    int m_physical_mapping_index;
    
    // @brief Maps this step to a position index in the sequence execution order.
    // This begins on the upper row and ends on the lower row, sweeping left to right
    int m_sequence_mapping_index;
    
};

} // namespace bass_station

#endif // __STEP_HPP__