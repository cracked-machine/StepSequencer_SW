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


#ifndef __NOTE_HPP__
#define __NOTE_HPP__

#include <static_map.hpp>
#include <adg2188.hpp>
#include <static_string.hpp>

namespace bass_station 
{

// @brief Represent each note on the BassStation keyboard
enum Note
{
    c0,          
    c0_sharp,
    d0,
    d0_sharp,
    e0,
    f0,
    f0_sharp,
    g0,
    g0_sharp,
    a1,
    a1_sharp,
    b1,
    c1,  // Middle C
    c1_sharp,
    d1,
    d1_sharp,
    e1,
    f1,
    f1_sharp,
    g1,
    g1_sharp,
    a2,
    a2_sharp,
    b2,
    c2,
    none,
};

// @brief Class to hold note string text and associated adg2188 pole config
class NoteData
{
public:
    NoteData(std::string note, noarch::containers::StaticString<4> note_static_string, adg2188::Driver::Pole sw)
    : m_note_string(note), m_note_static_string(note_static_string), m_sw(sw) {}
    std::string m_note_string;
    noarch::containers::StaticString<4> m_note_static_string;
    adg2188::Driver::Pole m_sw;
};

} // namespace bass_station
#endif // __NOTE_HPP__