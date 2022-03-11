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

#ifndef __FILE_MANAGER_HPP__
#define __FILE_MANAGER_HPP__

#include <ff_driver.hpp>
#include <array>

namespace bass_station 
{

class FileManager 
{
public:
    FileManager(fatfs::DiskioProtocolSPI &fatfs_spi_interface);

private:

    fatfs::DiskIO_MMC_SPI m_diskio_mmc_spi;
    fatfs::DriverSPI m_fat_handle;
    fatfs::FATFS m_filesys;
    fatfs::FRESULT m_last_result;

    // uSD device logical drive path
    std::array<fatfs::TCHAR, 4> m_sd_path;     

    
};

} // namespace bass_station 


#endif // __FILE_MANAGER_HPP__