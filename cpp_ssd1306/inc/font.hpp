

#ifndef __FONT_HPP__
#define __FONT_HPP__

#include <stdint.h>
#include <array>
//#include <variant>
//#include <fontdata.hpp>



namespace ssd1306
{

	void thing();

template<std::size_t FONT_SIZE>
class Font
{

public:

	// @brief Construct a new Font object
	Font() = default;

	// @brief function to get a font pixel (16bit half-word).
	// @param idx The position in the font data array to retrieve data
	// @return uint16_t The halfword of data we retrieve
	bool get_pixel(size_t idx, uint32_t &bit_line) 
	{ 
		if (idx > data.size())
		{
			return false;
		}
		else
		{
			bit_line = static_cast<uint32_t>(data.at(idx)); 
			return true;
		}
	}

	// @brief get the width member variable 
	// @return uint8_t the width value
	uint8_t width() { return m_width; }

	// @brief get tte height member variable 
	// @return uint8_t the height value
	uint8_t height() { return m_height; }

	// @brief helper function to get the size of the private font data array.
	// @return size_t the array size
	size_t size() { return data.size(); }

private:

	// @brief The width of the font in pixels
	static uint8_t m_width; 

	// @brief The height of the font in pixels
	static uint8_t m_height;

	// @brief the font data
	static std::array<uint16_t, FONT_SIZE> data;

};

// specializations
typedef Font<475> Font3x5;
typedef Font<680> Font5x7;
typedef Font<950> Font7x10;
typedef Font<1710> Font11x16;
typedef Font<2470> Font16x26;

} // namespace ssd1306

#endif // __FONT_HPP__
