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

#include <file_manager.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
    #include <stm32g0xx_ll_gpio.h>
#pragma GCC diagnostic pop

namespace bass_station 
{
/// @brief Construct a new File Manager object
/// @param dtype 
/// @param fatfs_spi_interface 
FileManager::FileManager(
    fatfs::DiskioProtocolSPI &fatfs_spi_interface) 
:   // init the mmc diskio layer with the STM32 SPI definitions (SPI_TypeDef/GPIOs)
    m_diskio_mmc_spi(fatfs_spi_interface),
    // init the fatfs::Driver with the mmc/spi diskio layer
    m_fat_spi_driver(m_diskio_mmc_spi)
{

    // Mount the file system. 1 = mount now
	m_last_result = m_fat_spi_driver.f_mount(&m_filesys, m_sd_path.data(), 1); 
	// while (m_last_result != fatfs::FRESULT::FR_OK);

    // Open the file
    fatfs::FIL fil;
	m_last_result = m_fat_spi_driver.f_open(&fil, m_sd_path.data(), fatfs::FA_WRITE);
    // while (m_last_result != fatfs::FRESULT::FR_OK);
	
    // Read from the file
    std::array<char, 100> read_buff;
	fatfs::UINT bytes_read {0};
	m_last_result = m_fat_spi_driver.f_read(&fil, read_buff.data(), read_buff.size(), &bytes_read);
	// while (m_last_result != fatfs::FRESULT::FR_OK);

    // Write to the file
	std::array<char, 100> write_buff;
	fatfs::UINT bytes_written {0};
	m_last_result = m_fat_spi_driver.f_write(&fil, write_buff.data(), write_buff.size(), &bytes_written);
    // while (m_last_result != fatfs::FRESULT::FR_OK);
    
}



} // namespace bass_station 