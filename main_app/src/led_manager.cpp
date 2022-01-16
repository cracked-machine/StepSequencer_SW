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

#include <led_manager.hpp>

namespace bass_station 
{

LedManager::LedManager(SPI_TypeDef *spi_handle)
{
    tlc5955_driver = tlc5955::Driver(spi_handle);
}

void LedManager::send_control_data()
{
	// refresh buffers
	tlc5955_driver.reset();
	
	// construct the SPI transmit data
	tlc5955_driver.set_ctrl_cmd();
	tlc5955_driver.set_padding_bits();
	tlc5955_driver.set_function_cmd(	
		tlc5955::Driver::DisplayFunction::display_repeat_off,
		tlc5955::Driver::TimingFunction::timing_reset_off,
		tlc5955::Driver::RefreshFunction::auto_refresh_off,
		tlc5955::Driver::PwmFunction::normal_pwm,
		tlc5955::Driver::ShortDetectFunction::threshold_90_percent
	);

	tlc5955_driver.set_global_brightness_cmd(0x1, 0x1, 0x1);
	tlc5955_driver.set_max_current_cmd(0x1, 0x1, 0x1);
	tlc5955_driver.set_dot_correction_cmd_all(0x7F);
	
	// prepare SPI transmit data as bytes
	tlc5955_driver.process_register();

	// send data for top row (no latch)
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::control);
	tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::no_latch);
	
	// send data for bottom row
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::control);
	tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);
}

void LedManager::set_all_leds(uint16_t greyscale_pwm, const LedColour &colour)
{
	// refresh buffers
	tlc5955_driver.reset();

    // send upper row first
    switch (colour)
    {
        case LedColour::red:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
        break;
        case LedColour::blue:
            tlc5955_driver.set_greyscale_cmd_rgb(0, 0, greyscale_pwm);
        break;		
        case LedColour::magenta:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
        break;	
        case LedColour::cyan:
            tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
        break;						             
    }

	// convert the bit buffer to bytes
	tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);

    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::no_latch);

    // send lower row second
    switch (colour)
    {
        case LedColour::red:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
        break;
        case LedColour::blue:
            tlc5955_driver.set_greyscale_cmd_rgb(0, 0, greyscale_pwm);
        break;		
        case LedColour::magenta:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
        break;	
        case LedColour::cyan:
            tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
        break;						             
    }

	// convert the bit buffer to bytes
	tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);
    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send); 

}

void LedManager::set_position_and_colour(uint16_t position, LedColour colour)
{
    uint16_t greyscale_pwm {0xFFFF};
    switch (colour)
    {
        case LedColour::red:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, 0, 0);
        break;
        case LedColour::green:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, greyscale_pwm, 0);
        break;
        case LedColour::blue:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, 0, greyscale_pwm);
        break;		
        case LedColour::magenta:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, 0, greyscale_pwm);
        break;						
        case LedColour::yellow:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, greyscale_pwm, greyscale_pwm, 0);
        break;	
        case LedColour::cyan:
            tlc5955_driver.set_greyscale_cmd_rgb_at_position(
                position, 0, greyscale_pwm, greyscale_pwm);
        break;						
    }

}

void LedManager::send_one_led_greyscale_data_at(uint16_t led_position, const SequencerRow &row, uint16_t greyscale_pwm, const LedColour &colour, const LatchOption &latch_option)
{
	// refresh buffers
	tlc5955_driver.reset();
	
	switch (row)
	{
        // set the colour and greyscale mapped to upper row indices
		case SequencerRow::upper:
			switch (colour)
			{
				case LedColour::red:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), greyscale_pwm, 0, 0);
				break;
				case LedColour::green:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), 0, greyscale_pwm, 0);
				break;
				case LedColour::blue:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), 0, 0, greyscale_pwm);
				break;		
				case LedColour::magenta:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), greyscale_pwm, 0, greyscale_pwm);
				break;						
				case LedColour::yellow:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), greyscale_pwm, greyscale_pwm, 0);
				break;	
				case LedColour::cyan:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(upper_row_led_mapping.at(led_position), 0, greyscale_pwm, greyscale_pwm);
				break;						
			}
		break;

		// set the colour and greyscale mapped to lower row indices
		case SequencerRow::lower:
			switch (colour)
			{
				case LedColour::red:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), greyscale_pwm, 0, 0);
				break;
				case LedColour::green:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), 0, greyscale_pwm, 0);
				break;
				case LedColour::blue:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), 0, 0, greyscale_pwm);
				break;		
				case LedColour::magenta:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), greyscale_pwm, 0, greyscale_pwm);
				break;		
				case LedColour::yellow:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), greyscale_pwm, greyscale_pwm, 0);
				break;		
				case LedColour::cyan:
					tlc5955_driver.set_greyscale_cmd_rgb_at_position(lower_row_led_mapping.at(led_position), 0, greyscale_pwm, greyscale_pwm);
				break;										
			}
		break;
	}
	    
	// convert the bit buffer to bytes
	tlc5955_driver.process_register();

	// send a first bit as 0 to notify chip this is  greyscale data
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);

    // now send the 96 bytes of greyscale data for this row (toggle the latch if requested)
	if (latch_option == LatchOption::enable) 
    { 
        tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send); 
    }
	else 
    { 
        tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::no_latch);
    }
}

void LedManager::clear_all_leds()
{
    tlc5955_driver.reset();

    // send upper row first with no latching
	tlc5955_driver.process_register();
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);
    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::no_latch); 

    // send lower row with latching
	tlc5955_driver.process_register();
	tlc5955_driver.send_first_bit(tlc5955::DataLatchType::greyscale);
    tlc5955_driver.send_spi_bytes(tlc5955::LatchPinOption::latch_after_send);
        
}



void LedManager::update_ladder_demo(uint16_t pwm_value, uint32_t delay_ms)
{

    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::red, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::red, LatchOption::enable);

        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::red, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::red, LatchOption::enable);	

        LL_mDelay(delay_ms);
    }	

    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::yellow, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::yellow, LatchOption::enable);
        
        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions	
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::yellow, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::yellow, LatchOption::enable);	

        LL_mDelay(delay_ms);
    }		

    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::green, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::green, LatchOption::enable);

        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::green, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::green, LatchOption::enable);	
        
        LL_mDelay(delay_ms);
    }	

    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::cyan, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::cyan, LatchOption::enable);
        
        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions	
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::cyan, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::cyan, LatchOption::enable);	

        LL_mDelay(delay_ms);
    }			
	
    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::blue, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::blue, LatchOption::enable);
        
        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions	
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::blue, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::blue, LatchOption::enable);	

        LL_mDelay(delay_ms);
    }	

    for (uint16_t led_position = 0; led_position<16; led_position++)
    {
        // set the LED colours and positions
        send_one_led_greyscale_data_at(led_position, SequencerRow::upper, pwm_value, LedColour::magenta, LatchOption::disable);
        send_one_led_greyscale_data_at(led_position, SequencerRow::lower, pwm_value, LedColour::magenta, LatchOption::enable);
        
        LL_mDelay(delay_ms);
    }

    for (int16_t led_position = 15; led_position > -1; led_position--)
    {
        // set the LED colours and positions	
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::upper, pwm_value, LedColour::magenta, LatchOption::disable);
        send_one_led_greyscale_data_at(static_cast<uint16_t>(led_position), SequencerRow::lower, pwm_value, LedColour::magenta, LatchOption::enable);	

        LL_mDelay(delay_ms);
    }	


}

} // namespace bass_station
