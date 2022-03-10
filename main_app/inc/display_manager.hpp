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

#ifndef __DISPLAY_MANAGER_HPP__
#define __DISPLAY_MANAGER_HPP__

#include <isr_manager_stm32g0.hpp>
#include <ssd1306.hpp>

namespace bass_station
{

// This class manages the SSD1306 driver for the 128x64 pixel OLED display
// DisplayManager::update_oled() function (redraws the screen) should be called from the main loop
class DisplayManager
{
public:

    // @brief Construct a new Display Manager object
    // @param timer Used for refresh rate of the display
    DisplayManager(ssd1306::DriverSerialInterface<stm32::isr::InterruptTypeStm32g0> &display_spi_interface);
    
    // @brief Abstract representation of a line of the display
    enum class DisplayLine
    {
        LINE_ONE,
        LINE_TWO,
        LINE_THREE,
        LINE_FOUR,
        LINE_FIVE,
        LINE_SIX,
    };

    // @brief Set a message to a specific line display (assumes Font5x7 size)
    // @param line The line to write to 
    // @param msg The text to write
    void set_display_line(DisplayLine line, std::string &msg);


    // @brief redraw the display
    void update_oled();

private:

    // set some default values
    std::string m_display_line1{"line1"};
    std::string m_display_line2{"line2"};
    std::string m_display_line3{"line3"};
    std::string m_display_line4{"line4"};
    std::string m_display_line5{"line5"};
    std::string m_display_line6{"line6"};

    // @brief The font character map used
    ssd1306::Font5x7 m_font;

    // @brief SSD1306 OLED driver
    ssd1306::Driver<stm32::isr::InterruptTypeStm32g0> m_oled;

};

} // namespace bass_station

#endif // __DISPLAY_MANAGER_HPP__


