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

#ifndef __SEQUENCE_MANAGER_HPP__
#define __SEQUENCE_MANAGER_HPP__

#include <display_manager.hpp>
#include <keypad_manager.hpp>
#include <led_manager.hpp>
#include <midi_stm32.hpp>

namespace bass_station
{

using tempo_timer_pair_t = std::pair<TIM_TypeDef *, STM32G0_ISR>;

// This class takes user key input and controls key LEDs (via LEDManager) and output synth control (via adg2188)
class SequenceManager
{
public:
  // clang-format off
    /// @brief Construct a new Sequence Manager
    /// @param sequencer_tempo_timer_pair The SequenceManager tempo timer (and its InterruptTypeStm32g0) for keeping
    /// tempo
    /// @param sequencer_encoder_timer The SequenceManager rotary encoder interface
    /// @param display_spi The DisplayManager SPI interface
    /// @param ad5587_keypad_i2c The KeypadManager I2C interface
    /// @param debounce_timer  General purpose debounce timer
    /// @param adg2188_control_sw_i2c The crosspoint switch I2C interface for controlling the synth notes
    /// @param led_spi_interface The LedManager SPI interface
    /// @param midi_usart_interface The MIDI USART interface
    SequenceManager(
        tempo_timer_pair_t tempo_timer_pair,
        TIM_TypeDef *sequencer_encoder_timer,
        ssd1306::DriverSerialInterface<STM32G0_ISR> &display_spi, 
        I2C_TypeDef *ad5587_keypad_i2c,
        TIM_TypeDef *debounce_timer,
        I2C_TypeDef *adg2188_control_sw_i2c,
        tlc5955::DriverSerialInterface &led_spi_interface,
        midi_stm32::DeviceInterface<STM32G0_ISR> &midi_usart_interface);
  // clang-format on
  /// @brief Start the main sequencer loop. Called from mainapp.cpp
  void main_loop();

private:
  // @brief List of operation modes for the sequencer
  enum class Mode
  {
    TEMPO_ADJUST, // @brief User can select tempo using rotary encoder (enabled after NOTE_SELECT timeout)
    NOTE_SELECT,  // @brief User can select note using rotary encoder (enabled after selecting step key)
  };

  // @brief The current mode (and its default)
  Mode m_current_mode{Mode::TEMPO_ADJUST};

  // @brief state variable for previous note
  NoteData *m_previous_enabled_note;

  // @brief The previously captured rotary encoder value
  uint16_t m_last_encoder_value;

  // @brief  The 32-step sequence data
  static std::array<std::pair<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, Step>, 32> m_sequence_data;

  /// @brief Map of key (ADP5587 HW button index) and values (Step object)
  // clang-format off
    noarch::containers::StaticMap<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, Step, m_sequence_data.size()> m_sequence_map = 
        noarch::containers::StaticMap<adp5587::Driver<STM32G0_ISR>::KeyPadMappings, Step, m_sequence_data.size()>{{m_sequence_data}};

  // clang-format on

  // @brief The 25-key note data of the BassStation keyboard
  static std::array<std::pair<Note, NoteData>, 25> m_note_switch_data;

  /// @brief Map holding the note data associated with its ADG2188 HW crosspoint switch config
  noarch::containers::StaticMap<Note, NoteData, m_note_switch_data.size()> m_note_switch_map =
      noarch::containers::StaticMap<Note, NoteData, m_note_switch_data.size()>{{m_note_switch_data}};

  /// @brief The timer for tempo of the sequencer
  tempo_timer_pair_t m_tempo_timer_pair;
  float m_tempo_timer_freq_hz{0};

  /// @brief The timer used for rotary encoder
  // std::unique_ptr<TIM_TypeDef> m_sequencer_encoder_timer;
  TIM_TypeDef *m_sequencer_encoder_timer;

  /// @brief Manages the SSD1306 OLED display
  bass_station::DisplayManager m_ssd1306_display_spi;

  /// @brief Manages the ADP5587 keyscanner/io expander chip
  bass_station::KeypadManager m_adp5587_keypad_i2c;

  /// @brief Manages the ADG2188 crosspoint switch chip
  adg2188::Driver m_synth_control_switch;

  /// @brief Manages the TLC5955 chip
  bass_station::LedManager m_led_manager;

  midi_stm32::Driver<STM32G0_ISR> m_midi_driver;

  /// @brief counter for sequencer position, incremented in increment_and_execute_sequence_step()
  uint8_t m_pattern_cursor{0};

  /// @brief Save this value so we can return to TEMPO_MODE with the expected tempo
  uint16_t m_saved_tempo_setting{0};

  /// @brief The timer for mode button debounce
  // std::unique_ptr<TIM_TypeDef> m_debounce_timer;
  TIM_TypeDef *m_debounce_timer;

  /// @brief The allowable delay between pressing keys on the sequence keypad
  /// Increasing this value will decrease bounce but also responsiveness
  const uint32_t m_mode_debounce_threshold_ms{350};
  /// @brief Store the last timer count for debounce
  uint32_t m_last_mode_debounce_count_ms{0};

  void led_demo();

  noarch::containers::StaticString<20> m_display_direction;

  /// @brief Update the display and tempo timer
  void update_display_and_tempo();

  /// @brief Runs the note/step sequence
  /// @param run_demo_only
  void execute_next_sequence_step();

  UserKeyStates m_midi_state{UserKeyStates::STOPPED};
  UserKeyStates m_sequencer_state{UserKeyStates::STOPPED};

  /// @brief This determines the positional order in which the cursor sweeps the sequence
  // This begins on the upper row and ends on the lower row, sweeping left to right
  std::array<uint8_t, 32> m_sequencer_key_mapping{16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                                  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15};

  /// @brief Registers Timer ISR handler class with InterruptManager for STM32G0
  struct TempoTimerIntHandler : public stm32::isr::InterruptManagerStm32Base<STM32G0_ISR>
  {
    /// @brief the parent driver class
    SequenceManager *m_seq_man_ptr;

    /// @brief initialise and register this handler instance with InterruptManagerStm32g0
    // @param parent_driver_ptr the instance to register
    void init_tempo_timer_callback(SequenceManager *seq_man_ptr)
    {
      m_seq_man_ptr = seq_man_ptr;
      // register pointer to this handler class in stm32::isr::InterruptManagerStm32g0
      stm32::isr::InterruptManagerStm32Base<STM32G0_ISR>::register_handler(m_seq_man_ptr->m_tempo_timer_pair.second, this);
    }
    // @brief Definition of InterruptManagerStm32Base::ISR. This is called by
    // stm32::isr::InterruptManagerStm32Base<sINTERRUPT_TYPE> specialization
    virtual void ISR() { m_seq_man_ptr->tempo_timer_isr(); }
  };
  /// @brief TempoTimerIntHandler member
  TempoTimerIntHandler m_sequencer_tempo_timer_isr_handler;

  /// @brief SequenceManager callback for timer interrupt
  void tempo_timer_isr();

  /// @brief Registers EXTI ISR handler class with InterruptManager for STM32G0
  struct RotarySwExtIntHandler : public stm32::isr::InterruptManagerStm32Base<STM32G0_ISR>
  {
    // @brief the parent driver class
    SequenceManager *m_parent_driver_ptr;
    // @brief initialise and register this handler instance with IsrManagerStm32g0
    // @param parent_driver_ptr the instance to register
    void init_rotary_encoder_callback(SequenceManager *parent_driver_ptr)
    {
      m_parent_driver_ptr = parent_driver_ptr;
      // register pointer to this handler class in stm32::isr::IsrManagerStm32g0
      stm32::isr::InterruptManagerStm32Base<STM32G0_ISR>::register_handler(STM32G0_ISR::exti10, this);
    }
    // @brief The callback used by IsrManagerStm32g0
    virtual void ISR() { m_parent_driver_ptr->rotary_sw_exti_isr(); }
  };
  // @brief handler object
  RotarySwExtIntHandler m_rotary_sw_exti_handler;

  /// @brief SequenceManager callback for exti15 interrupt
  void rotary_sw_exti_isr();
};

} // namespace bass_station

#endif // __SEQUENCE_MANAGER_HPP__