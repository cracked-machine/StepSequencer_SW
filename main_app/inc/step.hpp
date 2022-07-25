// MIT License

// Copyright (c) 2022 Chris Sutton

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

#include <adp5587.hpp>
#include <note.hpp>
#include <tlc5955.hpp>

#if defined(X86_UNIT_TESTING_ONLY)
#include <mock_cmsis.hpp>
#else
// used for LL_mDelay()
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic pop
#endif // X86_UNIT_TESTING_ONLY
namespace bass_station
{

// @brief Refers to the two TLC5955 chips in the bass station sequencer PCB
enum class SequencerRow
{
    upper,
    lower
};

// @brief Basic state for keys
// Not enum class so it can be used as boolean
enum KeyState
{
    ON,
    OFF,
};

constexpr tlc5955::LedColour default_colour{tlc5955::LedColour::white};
constexpr tlc5955::LedColour user_select_colour{tlc5955::LedColour::blue};
constexpr tlc5955::LedColour beat_colour_off{tlc5955::LedColour::white};
constexpr tlc5955::LedColour beat_colour_on{tlc5955::LedColour::red};

// @brief Represents a single step/key/button in the sequencer
struct Step
{

    // @brief Construct a new Step object
    // @param key_state ON or OFF
    // @param colour The colour when ON
    // @param logical_mapping_index Maps this step to a position index in one of two 16 position arrays.
    // @param physical_mapping_index Maps this step to the physical wiring pin index of the TLC5955 chip on the PCB
    // @param sequence_mapping_index Maps this step to a position index in the sequence execution order.
    Step(KeyState key_state, Note note, tlc5955::LedColour colour, int logical_mapping_index,
         int physical_mapping_index, int array_index)

        : m_key_state(key_state), m_note(note), m_colour(colour), m_logical_mapping_index(logical_mapping_index),
          m_physical_mapping_index(physical_mapping_index), m_array_index(array_index)
    {
        // nothing else to do here
    }

    // @brief Is the Key ON or OFF
    KeyState m_key_state;

    Note m_note;

    // @brief The colour of the key when it is ON
    tlc5955::LedColour m_colour;

    // @brief Maps this step to a position index in one of two 16 position arrays.
    int m_logical_mapping_index;

    // @brief Maps this step to the physical wiring pin index of the TLC5955 chip on the PCB
    int m_physical_mapping_index;

    // @brief Maps this step to a position index in the sequence execution order.
    // This begins on the upper row and ends on the lower row, sweeping left to right
    int m_array_index;
};

} // namespace bass_station

#endif // __STEP_HPP__