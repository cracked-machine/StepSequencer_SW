/*
 * Display.hpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

#ifndef Display_HPP_
#define Display_HPP_

#include <variant>
#include <font.hpp>
#include <sstream>
#include <iostream>
#include <array>
#include <utility>



#ifdef USE_HAL_DRIVER
	#include "stm32g0xx.h"
	#include "main.h"
	#include "spi.h"
#endif


namespace ssd1306
{

// @brief 
enum class Colour: uint16_t
{
    Black = 0x00, 
    White = 0x01  
};

// @brief 
class Display
{
public:
	Display() = default;

	// @brief 
	bool init(void);

	// @brief 
	// @param colour 
	void fill(Colour colour);

	// @brief 
	bool update_screen(void);

	// @brief 
	// @param x 
	// @param y 
	// @param colour 
	bool draw_pixel(uint8_t x, uint8_t y, Colour colour);

	// @brief 
	// @tparam FONT_SIZE 
	// @param msg 
	// @param font 
	// @param x 
	// @param y 
	// @param bg 
	// @param fg 
	// @param padding 
	// @param update 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write(std::stringstream &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, int padding, bool update);

	// @brief 
	// @tparam FONT_SIZE 
	// @param ss 
	// @param font 
	// @param colour 
	// @param padding 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour colour, int padding);

	// @brief 
	// @tparam FONT_SIZE 
	// @param ch 
	// @param font 
	// @param colour 
	// @param padding 
	// @return char 
	template<std::size_t FONT_SIZE> 
	char write_char(char ch, Font<FONT_SIZE> &font, Colour colour, int padding);
	
	// @brief Set the cursor object
	// @param x 
	// @param y 
	bool set_cursor(uint8_t x, uint8_t y);

	// @brief 
	void print_buffer_stdout();


private:
	
	// @brief 
	void reset(void);
	
	// @brief 
	// @param cmd_byte 
	bool write_command(uint8_t cmd_byte);
	
	// @brief 
	// @param data_buffer 
	// @param data_buffer_size 
	bool write_data(uint8_t* data_buffer, size_t data_buffer_size);

	// @brief 
    uint16_t m_currentx {0};

	// @brief 
    uint16_t m_currenty {0};

	// @brief 
    uint8_t m_inverted {0};

	// @brief 
    uint8_t m_initialized {0};

	// @brief 
    static constexpr uint16_t m_width {128};

	// @brief 
    static constexpr uint16_t m_height {64};

	// @brief 
    std::array<uint8_t, (m_width*m_height)/8> m_buffer;
	
#ifdef USE_HAL_DRIVER

	// @brief 
	SPI_HandleTypeDef m_spi_port {hspi1};
	// @brief 
	uint16_t m_cs_port {0};
	// @brief 
	uint16_t m_cs_pin {0};
	// @brief 
	GPIO_TypeDef* m_dc_port {SPI1_DC_GPIO_Port};
	// @brief 
	uint16_t m_dc_pin {SPI1_DC_Pin};
	// @brief 
	GPIO_TypeDef* m_reset_port {SPI1_RESET_GPIO_Port};
	// @brief 
	uint16_t m_reset_pin {SPI1_RESET_Pin};

#endif

};

// Out-of-class definitions of member function templates 

template<std::size_t FONT_SIZE>
char Display::write(std::stringstream &msg, Font<FONT_SIZE> &font, uint8_t x, uint8_t y, Colour bg, Colour fg, int padding, bool update)
{

    fill(bg);
    if (!set_cursor(x, y))
	{
		return 0;
	}
    char res = write_string(msg, font, fg, padding);
    if (update)
    {
        update_screen();
    }
    return res;
}

template<std::size_t FONT_SIZE>
char Display::write_char(char ch, Font<FONT_SIZE> &font, Colour color, int padding)
{

    // Check remaining space on current line
    if (m_width <= (m_currentx + font.height()) ||
        m_width <= (m_currenty + font.height()))
    {
        // Not enough space on current line
        return 0;
    }

    // add extra leading horizontal space
    if (padding == 1)
    {
    	for(size_t n = 0; n < font.height(); n++)
		{
			if (!draw_pixel(m_currentx, (m_currenty + n), Colour::Black))
			{
				return false;
			}
		}
    	m_currentx += 1;
    }


    // Use the font to write
    uint32_t b;
    for(size_t i = 0; i < font.height(); i++) {
        if (!font.get_pixel( (ch - 32) * font.height() + i, b )) { return false; }
        for(size_t j = 0; j < font.width(); j++) {
            if((b << j) & 0x8000)
            {
            	if (color == (Colour::White))
            	{
            		if (!draw_pixel(m_currentx + j, (m_currenty + i), Colour::White))
					{
						return false;
					}
            	}
            	else
            	{
            		if (!draw_pixel(m_currentx + j, (m_currenty + i), Colour::Black))
					{
						return false;
					}
            	}
            }
            else
            {
            	if (color == (Colour::White))
            	{
            		if (!draw_pixel(m_currentx + j, (m_currenty + i), Colour::Black))
					{
						return false;
					}
            	}
            	else
            	{
            		if (!draw_pixel(m_currentx + j, (m_currenty + i), Colour::White))
					{
						return false;
					}
            	}

            }
        }
    }

    // The current space is now taken
    m_currentx += font.width();
    // add extra leading horizontal space
    if (padding == 1)
    	m_currentx += 1;

    // Return written char for validation
    return ch;
}

template<std::size_t FONT_SIZE>
char Display::write_string(std::stringstream &ss, Font<FONT_SIZE> &font, Colour color, int padding)
{
    // Write until null-byte
	char ch;
    while (ss.get(ch))
    {
        if (write_char(ch, font, color, padding) != ch)
        {
            // Char could not be written
            return ch;
        }
    }

    // Everything ok
    return ch;
}

} // namespace ssd1306

#endif /* Display_HPP_ */
