/*
 * Display.cpp
 *
 *  Created on: 7 Nov 2021
 *      Author: chris
 */

// https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

#include "ssd1306.hpp"
#include <iomanip>
#include <bitset>

namespace ssd1306
{

bool Display::init(void)
{
    bool res = true;
	// Reset Display
	reset();

    // Wait for the screen to boot
#ifdef USE_HAL_DRIVER
    HAL_Delay(100);
#endif
    // Init Display
    if (!write_command(0xAE)) { return false; } //display off

    if (!write_command(0x20)) { return false; } //Set Memory Addressing Mode
    if (!write_command(0x10)) { return false; } // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode;
                                // 10,Page Addressing Mode (RESET); 11,Invalid

    if (!write_command(0xB0)) { return false; } //Set Page Start Address for Page Addressing Mode,0-7


    if (!write_command(0xC8)) { return false; } //Set COM Output Scan Direction


    if (!write_command(0x00)) { return false; } //---set low column address
    if (!write_command(0x10)) { return false; } //---set high column address

    if (!write_command(0x40)) { return false; } //--set start line address - CHECK

    if (!write_command(0x81)) { return false; } //--set contrast control register - CHECK
    if (!write_command(0xFF)) { return false; }


    if (!write_command(0xA1)) { return false; } //--set segment re-map 0 to 127 - CHECK



    if (!write_command(0xA6)) { return false; } //--set normal color


    if (!write_command(0xA8)) { return false; } //--set multiplex ratio(1 to 64) - CHECK
    if (!write_command(0x3F)) { return false; } //

    if (!write_command(0xA4)) { return false; } //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    if (!write_command(0xD3)) { return false; } //-set display offset - CHECK
    if (!write_command(0x00)) { return false; } //-not offset

    if (!write_command(0xD5)) { return false; } //--set display clock divide ratio/oscillator frequency
    if (!write_command(0xF0)) { return false; } //--set divide ratio

    if (!write_command(0xD9)) { return false; } //--set pre-charge period
    if (!write_command(0x22)) { return false; } //

    if (!write_command(0xDA)) { return false; } //--set com pins hardware configuration - CHECK
    if (!write_command(0x12)) { return false; }

    if (!write_command(0xDB)) { return false; } //--set vcomh
    if (!write_command(0x20)) { return false; } //0x20,0.77xVcc

    if (!write_command(0x8D)) { return false; } //--set DC-DC enable
    if (!write_command(0x14)) { return false; } //
    if (!write_command(0xAF)) { return false; } //--turn on Display panel

    // Clear screen
    fill(Colour::Black);

    // Flush buffer to screen
    update_screen();

    // Set default values for screen object
    m_currentx = 0;
    m_currenty = 0;

    m_initialized = 1;

    return res;
}


void Display::fill(Colour color)
{
    for(auto &pixel : m_buffer)
    {
        pixel = (color == Colour::Black) ? 0x00 : 0xFF;
    }
}

bool Display::update_screen(void)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if (!write_command(0xB0 + i)) { return false; }
        if (!write_command(0x00)) { return false; }
        if (!write_command(0x10)) { return false; }
        if (!write_data(&m_buffer[m_width * i], m_width)) { return false; }
    }
    return true;
}

bool Display::draw_pixel(uint8_t x, uint8_t y, Colour color)
{
    if(x >= m_width || y >= m_height) {
        // Don't write outside the buffer
        return false;
    }

    // Draw in the right color
    if(color == Colour::White) {
        m_buffer[x + (y / 8) * m_width] |= 1 << (y % 8);
    } else {
        m_buffer[x + (y / 8) * m_width] &= ~(1 << (y % 8));
    }

    return true;
}

bool Display::set_cursor(uint8_t x, uint8_t y)
{ 
    if(x >= m_width || y >= m_height) 
    {
        return false;
    }
    else
    {
        m_currentx = x;
        m_currenty = y;
    }
    return true;
}

void Display::print_buffer_stdout()
{


}


void Display::reset(void)
{
	// CS = High (not selected)
	//HAL_GPIO_WritePin(Display_CS_Port, Display_CS_Pin, GPIO_PIN_SET);

	// Reset the Display
#ifdef USE_HAL_DRIVER
	HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(m_reset_port, m_reset_pin, GPIO_PIN_SET);
	HAL_Delay(10);
#endif
}

bool Display::write_command(uint8_t cmd_byte __attribute__((unused)))
{
#ifdef USE_HAL_DRIVER
    HAL_StatusTypeDef res = HAL_OK;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_RESET); // command
	res = HAL_SPI_Transmit(&m_spi_port, (uint8_t *) &cmd_byte, 1, HAL_MAX_DELAY);
    if (res != HAL_OK)
    {
        return false;
    }
    return true;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_SET); // un-select Display
#else
    return true;
#endif
}

bool Display::write_data(uint8_t* data_buffer __attribute__((unused)), size_t data_buffer_size __attribute__((unused)))
{
#ifdef USE_HAL_DRIVER
    HAL_StatusTypeDef res = HAL_OK;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_RESET); // select Display
	HAL_GPIO_WritePin(m_dc_port, m_dc_pin, GPIO_PIN_SET); // data
	res = HAL_SPI_Transmit(&m_spi_port, data_buffer, data_buffer_size, HAL_MAX_DELAY);
    if (res != HAL_OK)
    {
        return false;
    }
    return true;
	//HAL_GPIO_WritePin(m_cs_port, m_cs_pin, GPIO_PIN_SET); // un-select Display
#else
    return true;
#endif



} 

} // namespace ssd1306
