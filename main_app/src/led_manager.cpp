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

#include <led_manager.hpp>

namespace bass_station 
{

LedManager::LedManager(tlc5955::DriverSerialInterface &serial_interface) : m_tlc5955_driver(tlc5955::Driver(serial_interface))
{
}

void LedManager::send_control_data()
{
	// refresh buffers
	m_tlc5955_driver.reset();
	
	// construct the SPI transmit data
	m_tlc5955_driver.set_ctrl_cmd();
	m_tlc5955_driver.set_padding_bits();
	m_tlc5955_driver.set_function_cmd(	
		tlc5955::Driver::DisplayFunction::display_repeat_off,
		tlc5955::Driver::TimingFunction::timing_reset_off,
		tlc5955::Driver::RefreshFunction::auto_refresh_off,
		tlc5955::Driver::PwmFunction::normal_pwm,
		tlc5955::Driver::ShortDetectFunction::threshold_90_percent
	);

	m_tlc5955_driver.set_global_brightness_cmd(0x1, 0x1, 0x1);
	m_tlc5955_driver.set_max_current_cmd(0x1, 0x1, 0x1);
	m_tlc5955_driver.set_dot_correction_cmd_all(0x1F);
	
	// prepare SPI transmit data as bytes
	m_tlc5955_driver.process_register();

	// send data for top row (no latch)
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::control);
	m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::no_latch);
	
	// send data for bottom row
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::control);
	m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::latch_after_send);
}

void LedManager::set_all_leds(uint16_t greyscale_pwm, const LedColour &colour)
{
	// refresh buffers
	m_tlc5955_driver.reset();

    // send upper row first
    switch (colour)
    {
        case LedColour::blue:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
        break;
        case LedColour::red:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
        break;		
        case LedColour::magenta:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
        break;	
        case LedColour::cyan:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
        break;		
        case LedColour::white:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
        break;						             
    }

	// convert the bit buffer to bytes
	m_tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);

    m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::no_latch);

    // send lower row second
    switch (colour)
    {
        case LedColour::blue:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
        break;
        case LedColour::red:
            m_tlc5955_driver.set_greyscale_cmd_rgb(0, 0, greyscale_pwm);
        break;		
        case LedColour::magenta:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
        break;	
        case LedColour::cyan:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
        break;						   
        case LedColour::white:
            m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
        break;						                               
    }

	// convert the bit buffer to bytes
	m_tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);
    m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::latch_after_send); 

}

void LedManager::set_position_and_colour(uint16_t position, LedColour colour)
{
    uint16_t greyscale_pwm {0xFFFF};
    switch (colour)
    {
        case LedColour::red:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, greyscale_pwm, 0);
        break;
        case LedColour::blue:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, 0, greyscale_pwm);
        break;		
        case LedColour::magenta:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, greyscale_pwm, 0);
        break;	
        case LedColour::cyan:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, greyscale_pwm, greyscale_pwm);
        break;	
        case LedColour::white:
            m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, greyscale_pwm, greyscale_pwm);
        break;	        					
    }

}

void LedManager::send_one_led_greyscale_data_at(uint16_t led_position, const SequencerRow &row, uint16_t greyscale_pwm, const LedColour &colour, const LatchOption &latch_option)
{
	// refresh buffers
	m_tlc5955_driver.reset();
	
	switch (row)
	{
        // set the colour and greyscale mapped to upper row indices
		case SequencerRow::upper:
			switch (colour)
			{
				case LedColour::red:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, 0);
				break;
				case LedColour::green:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, 0);
				break;
				case LedColour::blue:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, 0, greyscale_pwm);
				break;		
				case LedColour::magenta:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, greyscale_pwm);
				break;						
				case LedColour::yellow:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, greyscale_pwm, 0);
				break;	
				case LedColour::cyan:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, greyscale_pwm);
				break;		
                case LedColour::white:
                    m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
                break;						                             				
			}
		break;

		// set the colour and greyscale mapped to lower row indices
		case SequencerRow::lower:
			switch (colour)
			{
				case LedColour::red:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, 0);
				break;
				case LedColour::green:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, 0);
				break;
				case LedColour::blue:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, 0, greyscale_pwm);
				break;		
				case LedColour::magenta:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, greyscale_pwm);
				break;		
				case LedColour::yellow:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, greyscale_pwm, 0);
				break;		
				case LedColour::cyan:
					m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, greyscale_pwm);
				break;					
                case LedColour::white:
                    m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
                break;						                             					
			}
		break;
	}
	    
	// convert the bit buffer to bytes
	m_tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);

    // now send the 96 bytes of greyscale data for this row (toggle the latch if requested)
	if (latch_option == LatchOption::enable) 
    { 
        m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::latch_after_send); 
    }
	else 
    { 
        m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::no_latch);
    }
}

void LedManager::clear_all_leds()
{
    m_tlc5955_driver.reset();

    // send upper row first with no latching
	m_tlc5955_driver.process_register();
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);
    m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::no_latch); 

    // send lower row with latching
	m_tlc5955_driver.process_register();
	m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);
    m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::latch_after_send);
        
}




} // namespace bass_station
