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

LedManager::LedManager(tlc5955::DriverSerialInterface &serial_interface)
    : m_tlc5955_driver(tlc5955::Driver(serial_interface))
{
  m_tlc5955_driver.init();
}

void LedManager::reinit_driver(tlc5955::Driver::DisplayFunction display,
                               tlc5955::Driver::TimingFunction timing,
                               tlc5955::Driver::RefreshFunction refresh,
                               tlc5955::Driver::PwmFunction pwm,
                               tlc5955::Driver::ShortDetectFunction short_detect,
                               std::array<uint8_t, 3> global_brightness,
                               std::array<uint8_t, 3> max_current,
                               uint8_t global_dot_correction)
{
  m_tlc5955_driver.init(display, timing, refresh, pwm, short_detect, global_brightness, max_current, global_dot_correction);
}

void LedManager::set_all_leds_both_rows(uint16_t greyscale_pwm, const tlc5955::LedColour &colour)
{
  m_tlc5955_driver.clear_register();

  // send upper row first
  switch (colour)
  {
    case tlc5955::LedColour::blue:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
      break;
    case tlc5955::LedColour::green:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
      break;
    case tlc5955::LedColour::red:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, 0, greyscale_pwm);
      break;
    case tlc5955::LedColour::magenta:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
      break;
    case tlc5955::LedColour::yellow:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
      break;
    case tlc5955::LedColour::cyan:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
      break;
    case tlc5955::LedColour::white:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
      break;
  }

  // send a first bit as 0 to notify chip this is  greyscale data
  m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);

  m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::no_latch);

  // send lower row second
  switch (colour)
  {
    case tlc5955::LedColour::blue:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, 0);
      break;
    case tlc5955::LedColour::green:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, 0);
      break;
    case tlc5955::LedColour::red:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, 0, greyscale_pwm);
      break;
    case tlc5955::LedColour::magenta:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, 0, greyscale_pwm);
      break;
    case tlc5955::LedColour::yellow:
      m_tlc5955_driver.set_greyscale_cmd_rgb(0, greyscale_pwm, greyscale_pwm);
      break;
    case tlc5955::LedColour::cyan:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, 0);
      break;
    case tlc5955::LedColour::white:
      m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
      break;
  }

  // send a first bit as 0 to notify chip this is  greyscale data
  m_tlc5955_driver.send_first_bit(tlc5955::Driver::DataLatchType::data);
  m_tlc5955_driver.send_spi_bytes(tlc5955::Driver::LatchPinOption::latch_after_send);
}

void LedManager::set_one_led_at(
    uint16_t led_position, const SequencerRow &row, uint16_t greyscale_pwm, const tlc5955::LedColour &colour, const LatchOption &latch_option)
{
  m_tlc5955_driver.clear_register();

  switch (row)
  {
    // set the colour and greyscale mapped to upper row indices
    case SequencerRow::upper:
      switch (colour)
      {
        case tlc5955::LedColour::red:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, 0);
          break;
        case tlc5955::LedColour::green:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, 0);
          break;
        case tlc5955::LedColour::blue:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, 0, greyscale_pwm);
          break;
        case tlc5955::LedColour::magenta:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, greyscale_pwm);
          break;
        case tlc5955::LedColour::yellow:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, greyscale_pwm, 0);
          break;
        case tlc5955::LedColour::cyan:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, greyscale_pwm);
          break;
        case tlc5955::LedColour::white:
          m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
          break;
      }
      break;

    // set the colour and greyscale mapped to lower row indices
    case SequencerRow::lower:
      switch (colour)
      {
        case tlc5955::LedColour::red:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, 0);
          break;
        case tlc5955::LedColour::green:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, 0);
          break;
        case tlc5955::LedColour::blue:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, 0, greyscale_pwm);
          break;
        case tlc5955::LedColour::magenta:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, 0, greyscale_pwm);
          break;
        case tlc5955::LedColour::yellow:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, greyscale_pwm, greyscale_pwm, 0);
          break;
        case tlc5955::LedColour::cyan:
          m_tlc5955_driver.set_greyscale_cmd_rgb_at_position(led_position, 0, greyscale_pwm, greyscale_pwm);
          break;
        case tlc5955::LedColour::white:
          m_tlc5955_driver.set_greyscale_cmd_rgb(greyscale_pwm, greyscale_pwm, greyscale_pwm);
          break;
      }
      break;
  }

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

} // namespace bass_station
