
#include <stdint.h>
#include <bitset>

#ifdef USE_HAL_DRIVER
    #include "stm32g0xx.h"
    #include "main.h"
#endif


//#include "spi.h"

#include <ssd1306.hpp>

namespace tlc5955 {

// https://godbolt.org/z/1q9sn3Gar

class Driver
{
public:

    Driver() = default;

    static const uint8_t m_bc_data_resolution {7};
    static const uint8_t m_mc_data_resolution {3};
    static const uint8_t m_dc_data_resolution {7};
    static const uint8_t m_gs_data_resolution {16};

    void set_control_bit(bool ctrl);

    void set_ctrl_cmd_bits();

    void set_padding_bits();

    // @brief Set the Function Control (FC) data latch.
    // See section 8.3.2.7 "Function Control (FC) Data Latch" (page 23).
    // https://www.ti.com/lit/ds/symlink/tlc5955.pdf
    // @param DSPRPT Auto display repeat mode enable bit. When enabled each output repeats the PWM control every 65,536 GSCLKs.
    // @param TMGRST Display timing reset mode enable bit. When enabled the GS counter resets and outputs forced off at the latch rising edge
    // for a GS data write
    // @param RFRESH Data in the common register are copied to the GS data latch and DC data in the control data latch are copied to the DC data latch
    // at the 65,536th GSCLK after the LAT rising edge for a GS data write.
    // @param ESPWM When 0, conventional PWM is selected. When 1, Enhanced Spectrum (ES) PWM is selected. See 8.4.4 "Grayscale (GS) Function (PWM Control)"
    // @param LSDVLT LED short detection (LSD) detection voltage selection bit. When this bit is 0, the LSD voltage is VCC × 70%. 
    // When this bit is 1, the LSD voltage is VCC × 90%. See 8.3.5 "LED Short Detection (LSD)"
    void set_function_data(bool DSPRPT, bool TMGRST, bool RFRESH, bool ESPWM, bool LSDVLT);
  
    void set_bc_data(std::bitset<m_bc_data_resolution> &blue_value, 
        std::bitset<m_bc_data_resolution> &green_value, 
        std::bitset<m_bc_data_resolution> &red_value);

    void set_mc_data(std::bitset<m_mc_data_resolution> &blue_value, 
        std::bitset<m_mc_data_resolution> green_value, 
        std::bitset<m_mc_data_resolution> &red_value);

    // set the individual LED position to the DC values
    void set_dc_data(uint8_t led_idx, std::bitset<m_dc_data_resolution> &blue_value, 
    std::bitset<m_dc_data_resolution> &green_value, 
    std::bitset<m_dc_data_resolution> &red_value);

    // convenience function to set all LEDs to the same DC values
    void set_all_dc_data(std::bitset<m_dc_data_resolution> &blue_value, 
        std::bitset<m_dc_data_resolution> &green_value, 
        std::bitset<m_dc_data_resolution> &red_value);

    // set the individual LED position to the GS values
    void set_gs_data(uint8_t led_pos, std::bitset<m_gs_data_resolution> &blue_value, 
        std::bitset<m_gs_data_resolution> &green_value, 
        std::bitset<m_gs_data_resolution> &red_value);

    // convenience function to set all LEDs to the same GS values
    void set_all_gs_data(std::bitset<m_gs_data_resolution> &blue_value, 
        std::bitset<m_gs_data_resolution> &green_value, 
        std::bitset<m_gs_data_resolution> &red_value);

    void send_data();
    

    // void flush_common_register();
    void flush_common_register();

    // @brief toggle the latch pin terminal
    void toggle_latch();

    void print_common_bits();

    uint16_t startup_tests();

private:

    uint8_t built_in_test_fail {0};

    // Bits required for correct control reg size
    static const uint16_t m_common_reg_size_bits {769};
    static const uint8_t m_common_reg_size_bytes {97};

     // @brief The number of daisy chained driver chips in the circuit.
    uint8_t m_num_driver_ics {1}; 

    // @brief The number of colour channels per LED
    static const uint8_t m_num_colour_chan {3};
    
    // @brief The number of LEDs per driver chip
    static const uint8_t m_num_leds_per_chip {16};



    // the size of each common register section
    static const uint8_t m_latch_size_bits {1};                                                                             // 1U
    static const uint8_t m_ctrl_cmd_size_bits {8};                                                                          // 8U
    static constexpr uint16_t m_gs_data_one_led_size_bits {m_gs_data_resolution * m_num_colour_chan};                       // 48U
    static constexpr uint16_t m_gs_data_section_size_bits {m_gs_data_resolution * m_num_leds_per_chip * m_num_colour_chan}; // 768U
    static const uint8_t m_func_data_section_size_bits {5};                                                                 // 5U
    static constexpr uint8_t m_bc_data_section_size_bits {m_bc_data_resolution * m_num_colour_chan};                        // 21U
    static constexpr uint8_t m_mc_data_section_size_bits {m_mc_data_resolution * m_num_colour_chan};                        // 9U
    static constexpr uint8_t m_dc_data_one_led_size_bits {m_dc_data_resolution * m_num_colour_chan};                        // 21U
    static constexpr uint16_t m_dc_data_section_size_bits {m_dc_data_resolution * m_num_leds_per_chip * m_num_colour_chan}; // 336U
    static constexpr uint16_t m_padding_section_size_bits {                                                                 // 389U
        m_common_reg_size_bits  - m_latch_size_bits - m_ctrl_cmd_size_bits - m_func_data_section_size_bits - m_bc_data_section_size_bits - m_mc_data_section_size_bits - m_dc_data_section_size_bits
    };

    // the offset of each common register section
    static const uint8_t m_latch_offset {0};
    static constexpr uint8_t m_ctrl_cmd_offset {static_cast<uint8_t>(m_latch_offset + m_latch_size_bits)};                  // 1U
    static constexpr uint8_t m_gs_data_offset {static_cast<uint8_t>(m_ctrl_cmd_offset + m_ctrl_cmd_size_bits)};             // 9U - used in gs data latch only
    static constexpr uint8_t m_padding_offset {static_cast<uint8_t>(m_ctrl_cmd_offset + m_ctrl_cmd_size_bits)};             // 9U - used in ctrl data latch only
    static constexpr uint16_t m_func_data_offset {static_cast<uint16_t>(m_padding_offset + m_padding_section_size_bits)};   // 9U
    static constexpr uint16_t m_bc_data_offset {static_cast<uint16_t>(m_func_data_offset + m_func_data_section_size_bits)}; // 398U
    static constexpr uint16_t m_mc_data_offset {static_cast<uint16_t>(m_bc_data_offset + m_bc_data_section_size_bits)};     // 424U
    static constexpr uint16_t m_dc_data_offset {static_cast<uint16_t>(m_mc_data_offset + m_mc_data_section_size_bits)};     // 433U

void set_value_nth_bit(uint8_t &target, bool value, uint16_t shift_idx);



    
    std::array<uint8_t, m_common_reg_size_bytes> m_common_byte_register{0};
    std::bitset<m_common_reg_size_bits> m_common_bit_register{0};

    const uint8_t  m_latch_delay_ms {1};

    // @brief Predefined write command.
    // section 8.3.2.3 "Control Data Latch" (page 21).
    // section 8.3.2.2 "Grayscale (GS) Data Latch" (page 20).
    // https://www.ti.com/lit/ds/symlink/tlc5955.pdf
    std::bitset<8> m_ctrl_cmd {0x96};

    // @brief Predefined flush command
    std::bitset<8> m_flush_cmd {0};

    // void enable_spi();
    // void disable_spi();

    // void enable_gpio_output_only();
#ifdef USE_HAL_DRIVER 
    // @brief The HAL SPI interface
    SPI_HandleTypeDef m_spi_interface {hspi2};
    // @brief Latch GPIO pin
	uint16_t m_lat_pin {TLC5955_SPI2_LAT_Pin};
    // @brief Latch terminal GPIO port
    GPIO_TypeDef* m_lat_port {TLC5955_SPI2_LAT_GPIO_Port};
    // @brief GreyScale clock GPIO pin
    uint16_t m_gsclk_pin {TLC5955_SPI2_GSCLK_Pin};
    // @brief GreyScale clock GPIO port
    GPIO_TypeDef* m_gsclk_port {TLC5955_SPI2_GSCLK_GPIO_Port};
    // @brief SPI MOSI GPIO pin
    uint16_t m_mosi_pin {TLC5955_SPI2_MOSI_Pin};
    // @brief SPI MOSI GPIO port
    GPIO_TypeDef* m_mosi_port {TLC5955_SPI2_MOSI_GPIO_Port};
    // @brief SPI Clock GPIO pin
    uint16_t m_sck_pin {TLC5955_SPI2_SCK_Pin};
    // @brief SPI Clock GPIO port
    GPIO_TypeDef* m_sck_port {TLC5955_SPI2_SCK_GPIO_Port};
#endif	

};

} // tlc5955