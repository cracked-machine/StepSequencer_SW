
#include "tlc5955.hpp"
#include <sstream>
#include <cmath>
#include <cstring>
#ifdef USE_RTT
    #include <SEGGER_RTT.h>
#endif
namespace tlc5955 
{

uint16_t Driver::startup_tests()
{
    // latch bit test
    if (m_common_byte_register[0] != 0b00000000) built_in_test_fail++;       
    set_control_bit(true);
    if (m_common_byte_register[0] != 0b10000000) built_in_test_fail++;      // 128
    
    // control byte test

    // Ctrl      10010110  
    // bits      [======]
    // Bytes     ======][
    //             #0    #1

    set_ctrl_cmd_bits();
    if (m_common_byte_register[0] != 0b11001011) built_in_test_fail++;      // 203

    // padding bits test - bytes 1-48 should be empty
    set_padding_bits();
    for (uint8_t idx = 1; idx < 49; idx++)
    {
        if (m_common_byte_register[idx] != 0) { built_in_test_fail++; }
    }

    // function bits test   
    // bits      [===]
    //           =][==
    // Bytes   #49  #50

    set_function_data(true, false, false, false, false);    
    if (m_common_byte_register[49] != 0b00000010) built_in_test_fail++;   // 2
    set_function_data(true, true, false, false, false);
    if (m_common_byte_register[49] != 0b00000011) built_in_test_fail++;   // 3
    set_function_data(true, true, true, false, false);
    if (m_common_byte_register[50] != 0b10000000) built_in_test_fail++;   // 128
    set_function_data(true, true, true, true, false);
    if (m_common_byte_register[50] != 0b11000000) built_in_test_fail++;   // 192
    set_function_data(true, true, true, true, true);
    if (m_common_byte_register[50] != 0b11100000) built_in_test_fail++;   // 224

    // BC bits test
    // BC         blue   green   red
    // bits      [=====][=====][=====]
    // bits      ====][======][======]
    // Bytes     #50    #51      #52

    std::bitset<m_bc_data_resolution> bc_test_on {127};
    std::bitset<m_bc_data_resolution> bc_test_off {0};

    if (m_common_byte_register[50] != 0b11100000) built_in_test_fail++;     // 224
    if (m_common_byte_register[51] != 0b00000000) built_in_test_fail++;  
    if (m_common_byte_register[52] != 0b00000000) built_in_test_fail++;   

    set_bc_data(bc_test_on, bc_test_off, bc_test_off);
    if (m_common_byte_register[50] != 0b11111111) built_in_test_fail++;     // 255
    if (m_common_byte_register[51] != 0b11000000) built_in_test_fail++;     // 192
    if (m_common_byte_register[52] != 0x00000000) built_in_test_fail++;   

    set_bc_data(bc_test_off, bc_test_on, bc_test_off);
    if (m_common_byte_register[50] != 0b11100000) built_in_test_fail++;     // 224
    if (m_common_byte_register[51] != 0b00111111) built_in_test_fail++;     // 63
    if (m_common_byte_register[52] != 0b10000000) built_in_test_fail++;     // 128
    
    set_bc_data(bc_test_off, bc_test_off, bc_test_on);
    if (m_common_byte_register[50] != 0b11100000) built_in_test_fail++;     // 224
    if (m_common_byte_register[51] != 0x00000000) built_in_test_fail++;   
    if (m_common_byte_register[52] != 0b01111111) built_in_test_fail++;     // 127

    set_bc_data(bc_test_off, bc_test_off, bc_test_off);
    if (m_common_byte_register[50] != 0b11100000) built_in_test_fail++;     // 224
    if (m_common_byte_register[51] != 0b00000000) built_in_test_fail++;   
    if (m_common_byte_register[52] != 0b00000000) built_in_test_fail++;    

    set_bc_data(bc_test_on, bc_test_on, bc_test_on);
    if (m_common_byte_register[50] != 0b11111111) built_in_test_fail++;     // 255
    if (m_common_byte_register[51] != 0b11111111) built_in_test_fail++;     // 255
    if (m_common_byte_register[52] != 0b11111111) built_in_test_fail++;     // 255   

    // MC         B  G  R
    // bits      [=][=][=]
    // bits      [======][
    // Bytes       #53    #54

    std::bitset<m_mc_data_resolution> mc_test_on {7};
    std::bitset<m_mc_data_resolution> mc_test_off {0};
    set_mc_data(mc_test_on, mc_test_off, mc_test_off);
    if (m_common_byte_register[53] != 0b11100000) built_in_test_fail++;     // 224
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_on, mc_test_off);
    if (m_common_byte_register[53] != 0b00011100) built_in_test_fail++;     // 28
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_off, mc_test_on);
    if (m_common_byte_register[53] != 0b00000011) built_in_test_fail++;     // 3
    if (m_common_byte_register[54] != 0b10000000) built_in_test_fail++;     // 128

    set_mc_data(mc_test_off, mc_test_off, mc_test_off);
    if (m_common_byte_register[53] != 0b00000000) built_in_test_fail++;     
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     

    std::bitset<m_mc_data_resolution> mc_test_blue {1};
    std::bitset<m_mc_data_resolution> mc_test_green {1};
    std::bitset<m_mc_data_resolution> mc_test_red {1};
    set_mc_data(mc_test_off, mc_test_off, mc_test_red);   
    if (m_common_byte_register[53] != 0b00000000) built_in_test_fail++;     // 0   
    if (m_common_byte_register[54] != 0b10000000) built_in_test_fail++;     // 128
    set_mc_data(mc_test_off, mc_test_off, mc_test_red <<= 1);
    if (m_common_byte_register[53] != 0b00000001) built_in_test_fail++;     // 1
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_off, mc_test_red <<= 1);
    if (m_common_byte_register[53] != 0b00000010) built_in_test_fail++;     // 2
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_green, mc_test_red <<= 1);
    if (m_common_byte_register[53] != 0b00000100) built_in_test_fail++;     // 4
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_green <<= 1, mc_test_red);
    if (m_common_byte_register[53] != 0b00001000) built_in_test_fail++;     // 8
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_off, mc_test_green <<= 1, mc_test_red);
    if (m_common_byte_register[53] != 0b00010000) built_in_test_fail++;     // 16
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0
    set_mc_data(mc_test_blue, mc_test_green <<= 1, mc_test_red);
    if (m_common_byte_register[53] != 0b00100000) built_in_test_fail++;     // 32
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0    
    set_mc_data(mc_test_blue <<= 1, mc_test_green, mc_test_red);
    if (m_common_byte_register[53] != 0b01000000) built_in_test_fail++;     // 64
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0    
    set_mc_data(mc_test_blue <<= 1, mc_test_green, mc_test_red);
    if (m_common_byte_register[53] != 0b10000000) built_in_test_fail++;     // 128
    if (m_common_byte_register[54] != 0b00000000) built_in_test_fail++;     // 0    



    return built_in_test_fail;
}

void Driver::set_value_nth_bit(uint8_t &target, bool value, uint16_t shift_idx)
{
    if (value) { target |= (1U << shift_idx); }
    else { target &= ~(1U << shift_idx); }
    print_common_bits(); 
}


void Driver::set_control_bit(bool ctrl_latch)
{
    // Latch       
    // bits      =
    // Bytes     [
    //          #0  

    //m_common_bit_register.set(m_latch_offset, ctrl_latch);
    set_value_nth_bit(m_common_byte_register[0], ctrl_latch, 7);
}

void Driver::set_ctrl_cmd_bits()
{

    // Ctrl      10010110  
    // bits      [======]
    // Bytes     ======][
    //             #0    #1
   
    // 7 MSB bits of ctrl byte into 7 LSB of byte #0
    for (int8_t idx = m_ctrl_cmd_size_bits - 1; idx > 0; idx--)
    {
        set_value_nth_bit(m_common_byte_register[0], m_ctrl_cmd.test(idx), idx -1 );  
         
    }

    // the last m_ctrl_cmd bit in to MSB of byte #1
    set_value_nth_bit(m_common_byte_register[1], m_ctrl_cmd.test(0), 7);
    
}

void Driver::set_padding_bits()
{

    // Padding  0  ================================================================================ 79
    // Bytes       ======][======][======][======][======][======][======][======][======][======][
    //               #1      #2       #3    #4      #5      #6      #7      #8      #9     #10

    // Padding 80  ================================================================================ 159
    // Bytes       ======][======][======][======][======][======][======][======][======][======][
    //              #11     #12     #13     #14     #15     #16     #17     #18     #19     #20

    // Padding 160 ================================================================================ 239
    // Bytes       ======][======][======][======][======][======][======][======][======][======][
    //              #21     #22     #23     #24     #25     #26     #27     #28     #29     #30

    // Padding 240 ================================================================================ 319
    // Bytes       ======][======][======][======][======][======][======][======][======][======][
    //              #31     #32     #33     #34     #35     #36     #37     #38     #39     #40

    // Padding 320 ===================================================================== 389
    // Bytes       ======][======][======][======][======][======][======][======][=====
    //              #41     #42     #43     #44     #45     #46     #47     #48     #49

    // first, we write 7 LSB bits of m_common_byte_register[1] = 0
    for (int8_t idx = 6; idx > -1; idx--)
    {
        set_value_nth_bit(m_common_byte_register[1], false, idx);  
    }

    // The next 47 bytes are don't care padding = 0
    const uint16_t padding_bytes_remaining = 47U;
    for (uint16_t byte_idx  = 2; byte_idx < padding_bytes_remaining; byte_idx++)
    {
        for (int8_t bit_idx = 7; bit_idx > -1; bit_idx--)
        {
            set_value_nth_bit(m_common_byte_register[byte_idx], false, bit_idx);
        }
    }

    // lastly, we write 6 MSB bits of m_common_byte_register[49] = 0
    for (int8_t idx = 7; idx > 1; idx--)
    {
        set_value_nth_bit(m_common_byte_register[49], false, idx);    
    }

}

void Driver::set_function_data(bool DSPRPT, bool TMGRST, bool RFRESH, bool ESPWM, bool LSDVLT)
{ 

    // Function   
    // bits      [===]
    //           =][==
    // Bytes   #49  #50

    // if all are set to true, byte #49 = 3, byte #50 = 224
    set_value_nth_bit(m_common_byte_register[49], DSPRPT, 1);
    set_value_nth_bit(m_common_byte_register[49], TMGRST, 0);
    set_value_nth_bit(m_common_byte_register[50], RFRESH, 7);
    set_value_nth_bit(m_common_byte_register[50], ESPWM, 6);
    set_value_nth_bit(m_common_byte_register[50], LSDVLT, 5);
}

void Driver::set_bc_data(std::bitset<m_bc_data_resolution> &blue_value, 
    std::bitset<m_bc_data_resolution> &green_value, 
    std::bitset<m_bc_data_resolution> &red_value)
{
    // BC         blue   green   red
    // bits      [=====][=====][=====]
    // bits      ====][======][======]
    // Bytes     #50    #51      #52

    // set 5 LSB of byte #50 to bits 6-2 of BC blue_value
    for (int8_t bit_idx = m_bc_data_resolution - 1; bit_idx > 1; bit_idx--)
    {
        // offset the bit position in byte #50 by 2 places.
        set_value_nth_bit(m_common_byte_register[50], blue_value.test(bit_idx), bit_idx - 2);
    }

    // set the first 2 MSB bits of byte #51 to the last 2 LSB of blue_value
    set_value_nth_bit(m_common_byte_register[51], blue_value.test(1), 7);
    set_value_nth_bit(m_common_byte_register[51], blue_value.test(0), 6);

    // set 5 LSB of byte #51 to bits 6-1 of BC green_value
    for (int8_t bit_idx = m_bc_data_resolution - 1; bit_idx > 0; bit_idx--)
    {
        // offset the bit position in byte #51 by 1 places.
        set_value_nth_bit(m_common_byte_register[51], green_value.test(bit_idx), bit_idx - 1);
    }

    // set MSB of byte#52 to LSB of green_value
    set_value_nth_bit(m_common_byte_register[52], green_value.test(0), 7);

    // set 7 LSB of byte #50 to bits all 7 bits of BC red_value
    for (int8_t bit_idx = m_bc_data_resolution - 1; bit_idx > -1; bit_idx--)
    {
        // No offset for bit position in byte #52. 
        set_value_nth_bit(m_common_byte_register[52], red_value.test(bit_idx), bit_idx);
    }    

}

void Driver::set_mc_data(std::bitset<m_mc_data_resolution> &blue_value, 
    std::bitset<m_mc_data_resolution> green_value, 
    std::bitset<m_mc_data_resolution> &red_value)
{
    // MC         B  G  R
    // bits      [=][=][=]
    // bits      [======][
    // Bytes       #53    #54

    // 3 bits of blue in 3 MSB of byte #51 == 128
    set_value_nth_bit(m_common_byte_register[53], blue_value.test(m_mc_data_resolution - 1), 7);
    set_value_nth_bit(m_common_byte_register[53], blue_value.test(m_mc_data_resolution - 2), 6);
    set_value_nth_bit(m_common_byte_register[53], blue_value.test(m_mc_data_resolution - 3), 5);

    // 3 bits of green in next 3 bits of byte #51 == 144
    set_value_nth_bit(m_common_byte_register[53], green_value.test(m_mc_data_resolution - 1), 4);
    set_value_nth_bit(m_common_byte_register[53], green_value.test(m_mc_data_resolution - 2), 3);
    set_value_nth_bit(m_common_byte_register[53], green_value.test(m_mc_data_resolution - 3), 2);

    // 3 bits of red in 2 LSB of byte #51 (== 146) and MSB of byte #52 (== 0)
    set_value_nth_bit(m_common_byte_register[53], red_value.test(m_mc_data_resolution - 1), 1);
    set_value_nth_bit(m_common_byte_register[53], red_value.test(m_mc_data_resolution - 2), 0);
    set_value_nth_bit(m_common_byte_register[54], red_value.test(m_mc_data_resolution - 3), 7);

}

void Driver::set_dc_data(const uint8_t led_idx, std::bitset<m_dc_data_resolution> &blue_value, 
    std::bitset<m_dc_data_resolution> &green_value, 
    std::bitset<m_dc_data_resolution> &red_value)
{

    // DC        B15    G15    R15    B14    G14    R14    B13    G13    R13    B12    G12    R12
    // bits    [=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====]
    // Bytes   ======][======][======][======][======][======][======][======][======][======][====
    //          #54     #55     #56     #57      #58     #59    #60     #61     #62     #63     #64

    // DC        B11    G11    R11    B10    G10    R10    B9     G9     R9     B8     G8     R8
    // bits    [=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====]
    // Bytes   ==][======][======][======][======][======][======][======][======][======][======][
    //        #64   #65      #66    #67     #68     #69     #70     #71     #72     #73     #74

    // DC        B7      G7     R7    B6     G6     R6     B5     G5     R5     B4     G4     R4
    // bits    [=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====]
    // Bytes   ======][======][======][======][======][======][======][======][======][======][====
    //          #75     #76     #77     #78     #79     #80     #81     #82     #83     #84     #85

    


    switch(led_idx)
    {
        case 0:

            set_value_nth_bit(m_common_byte_register[93], blue_value.test(6), 3);
            set_value_nth_bit(m_common_byte_register[93], blue_value.test(5), 2);
            set_value_nth_bit(m_common_byte_register[93], blue_value.test(4), 1);
            set_value_nth_bit(m_common_byte_register[93], blue_value.test(3), 0);
            set_value_nth_bit(m_common_byte_register[94], blue_value.test(2), 7);
            set_value_nth_bit(m_common_byte_register[94], blue_value.test(1), 6);
            set_value_nth_bit(m_common_byte_register[94], blue_value.test(0), 5);

            set_value_nth_bit(m_common_byte_register[94], green_value.test(6), 4);
            set_value_nth_bit(m_common_byte_register[94], green_value.test(5), 3);
            set_value_nth_bit(m_common_byte_register[94], green_value.test(4), 2);
            set_value_nth_bit(m_common_byte_register[94], green_value.test(3), 1);
            set_value_nth_bit(m_common_byte_register[94], green_value.test(2), 0);
            set_value_nth_bit(m_common_byte_register[95], green_value.test(1), 7);
            set_value_nth_bit(m_common_byte_register[95], green_value.test(0), 6);

            set_value_nth_bit(m_common_byte_register[95], red_value.test(6), 5);
            set_value_nth_bit(m_common_byte_register[95], red_value.test(5), 4);
            set_value_nth_bit(m_common_byte_register[95], red_value.test(4), 3);
            set_value_nth_bit(m_common_byte_register[95], red_value.test(3), 2);
            set_value_nth_bit(m_common_byte_register[95], red_value.test(2), 1);
            set_value_nth_bit(m_common_byte_register[95], red_value.test(1), 0);
            set_value_nth_bit(m_common_byte_register[96], red_value.test(0), 7);

            break;



        case 1:
            set_value_nth_bit(m_common_byte_register[90], blue_value.test(6), 0);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(5), 7);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(4), 6);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(3), 5);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(2), 4);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(1), 3);
            set_value_nth_bit(m_common_byte_register[91], blue_value.test(0), 2);

            set_value_nth_bit(m_common_byte_register[91], green_value.test(6), 1);
            set_value_nth_bit(m_common_byte_register[91], green_value.test(5), 0);
            set_value_nth_bit(m_common_byte_register[92], green_value.test(4), 7);
            set_value_nth_bit(m_common_byte_register[92], green_value.test(3), 6);
            set_value_nth_bit(m_common_byte_register[92], green_value.test(2), 5);
            set_value_nth_bit(m_common_byte_register[92], green_value.test(1), 4);
            set_value_nth_bit(m_common_byte_register[92], green_value.test(0), 3);

            set_value_nth_bit(m_common_byte_register[92], red_value.test(6), 2);
            set_value_nth_bit(m_common_byte_register[92], red_value.test(5), 1);
            set_value_nth_bit(m_common_byte_register[92], red_value.test(4), 0);
            set_value_nth_bit(m_common_byte_register[93], red_value.test(3), 7);
            set_value_nth_bit(m_common_byte_register[93], red_value.test(2), 6);
            set_value_nth_bit(m_common_byte_register[93], red_value.test(1), 5);
            set_value_nth_bit(m_common_byte_register[93], red_value.test(0), 4);

            break;
        case 2:

            set_value_nth_bit(m_common_byte_register[88], blue_value.test(6), 5);
            set_value_nth_bit(m_common_byte_register[88], blue_value.test(5), 4);
            set_value_nth_bit(m_common_byte_register[88], blue_value.test(4), 3);
            set_value_nth_bit(m_common_byte_register[88], blue_value.test(3), 2);
            set_value_nth_bit(m_common_byte_register[88], blue_value.test(2), 1);
            set_value_nth_bit(m_common_byte_register[88], blue_value.test(1), 0);
            set_value_nth_bit(m_common_byte_register[89], blue_value.test(0), 7);

            set_value_nth_bit(m_common_byte_register[89], green_value.test(6), 6);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(5), 5);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(4), 4);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(3), 3);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(2), 2);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(1), 1);
            set_value_nth_bit(m_common_byte_register[89], green_value.test(0), 0);

            set_value_nth_bit(m_common_byte_register[90], red_value.test(6), 7);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(5), 6);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(4), 5);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(3), 4);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(2), 3);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(1), 2);
            set_value_nth_bit(m_common_byte_register[90], red_value.test(0), 1);
            
            break;
        case 3:

    // DC        B3     G3     R3      B2     G2    R2      B1    G1     R1     B0     G0     R0
    // bits    [=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====][=====]
    // Bytes   ==][======][======][======][======][======][======][======][======][======][======][
    //        #85   #86     #87      #88    #89     #90     #91     #92     #93     #94     #95   #96

            set_value_nth_bit(m_common_byte_register[85], blue_value.test(6), 2);
            set_value_nth_bit(m_common_byte_register[85], blue_value.test(5), 1);
            set_value_nth_bit(m_common_byte_register[85], blue_value.test(4), 0);
            set_value_nth_bit(m_common_byte_register[86], blue_value.test(3), 7);
            set_value_nth_bit(m_common_byte_register[85], blue_value.test(2), 6);
            set_value_nth_bit(m_common_byte_register[85], blue_value.test(1), 5);
            set_value_nth_bit(m_common_byte_register[85], blue_value.test(0), 4);

            

            

            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            break;

    }

}

void Driver::set_all_dc_data(std::bitset<m_dc_data_resolution> &blue_value, 
    std::bitset<m_dc_data_resolution> &green_value, 
    std::bitset<m_dc_data_resolution> &red_value)
{
    for (uint8_t led_idx = 0; led_idx < m_num_leds_per_chip; led_idx++)
    {
        set_dc_data(led_idx, blue_value, green_value, red_value);
    }
}

void Driver::set_gs_data(uint8_t led_pos, std::bitset<16> &blue_value, std::bitset<16> &green_value, std::bitset<16> &red_value)
{
    // offset for the current LED position
    const uint16_t led_offset = m_gs_data_one_led_size_bits * led_pos;

    // the current bit position within the GS section of the common register, starting at the section offset + LED offset
    uint16_t gs_common_pos = m_gs_data_offset + led_offset;

    // add each blue_value bit into the BC section of the common register
    for (uint8_t idx = 0; idx < blue_value.size(); idx++)
    {
        // make sure we stay within bounds of the common register
        if (gs_common_pos < m_common_reg_size_bits) 
        {
           m_common_bit_register.set(gs_common_pos, blue_value[idx]);
           gs_common_pos++;
        }
    }    

    // add each green_value bit into the GS section of the common register
    for (uint8_t idx = 0; idx < green_value.size(); idx++)
    {
        // make sure we stay within bounds of the common register
        if (gs_common_pos < m_common_reg_size_bits) 
        {
           m_common_bit_register.set(gs_common_pos, green_value[idx]);
           gs_common_pos++;
        }
    }    

    // add each red_value bit into the GS section of the common register
    for (uint8_t idx = 0; idx < red_value.size(); idx++)
    {
        // make sure we stay within bounds of the common register
        if (gs_common_pos < m_common_reg_size_bits) 
        {
           m_common_bit_register.set(gs_common_pos, red_value[idx]);
           gs_common_pos++;
        }
    }    
}

void Driver::set_all_gs_data(std::bitset<m_gs_data_resolution> &blue_value, 
    std::bitset<m_gs_data_resolution> &green_value, 
    std::bitset<m_gs_data_resolution> &red_value)
{
    for (uint8_t led_idx = 0; led_idx < m_num_leds_per_chip; led_idx++)
    {
        set_gs_data(led_idx, blue_value, green_value, red_value);
    }
}



void Driver::send_data()
{
    // clock the data through and latch
#ifdef USE_HAL_DRIVER
    HAL_StatusTypeDef res = HAL_SPI_Transmit(&m_spi_interface, (uint8_t*)m_common_byte_register.data(), m_common_reg_size_bytes, HAL_MAX_DELAY);
    UNUSED(res);
#endif
    toggle_latch();
}

void Driver::toggle_latch()
{
#ifdef USE_HAL_DRIVER
    HAL_Delay(m_latch_delay_ms);
    HAL_GPIO_WritePin(m_lat_port, m_lat_pin, GPIO_PIN_SET);
    HAL_Delay(m_latch_delay_ms);
    HAL_GPIO_WritePin(m_lat_port, m_lat_pin, GPIO_PIN_RESET);
    HAL_Delay(m_latch_delay_ms);
#endif
}

void Driver::flush_common_register()
{
    m_common_bit_register.reset();
    send_data();
}

void Driver::print_common_bits()
{
#ifdef USE_RTT
    SEGGER_RTT_printf(0, "\r\n");
    for (uint16_t idx = 45; idx < 53; idx++)
    {
        SEGGER_RTT_printf(0, "%u ", +m_common_byte_register[idx]);
    }
#endif
}

// void Driver::flush_common_register()
// {
//     // reset the latch
//     HAL_GPIO_WritePin(m_lat_port, m_lat_pin, GPIO_PIN_RESET);

//     // clock-in the entire common shift register per daisy-chained chip before pulsing the latch
//     for (uint8_t shift_entire_reg = 0; shift_entire_reg < m_num_driver_ics; shift_entire_reg++)
//     {
//         // write the MSB bit low to signal greyscale data
//         HAL_GPIO_WritePin(m_sck_port, m_sck_pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(m_mosi_port, m_mosi_pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(m_sck_port, m_sck_pin, GPIO_PIN_SET);
//         HAL_GPIO_WritePin(m_sck_port, m_sck_pin, GPIO_PIN_RESET);

//         // Set all 16-bit colours to 0 greyscale
//         uint8_t grayscale_data[2] = {0x00, 0x00};
//         for (uint8_t idx = 0; idx < 16; idx++)
//         {
//             HAL_SPI_Transmit(&m_spi_interface, grayscale_data, 2, HAL_MAX_DELAY);
//             HAL_SPI_Transmit(&m_spi_interface, grayscale_data, 2, HAL_MAX_DELAY);
//             HAL_SPI_Transmit(&m_spi_interface, grayscale_data, 2, HAL_MAX_DELAY);
//         }
//     }

//     toggle_latch();
// }

// void Driver::enable_spi()
// {
//     HAL_GPIO_DeInit(GPIOB, TLC5955_SPI2_MOSI_Pin|TLC5955_SPI2_SCK_Pin);

//     m_spi_interface.Instance = SPI2;
//     m_spi_interface.Init.Mode = SPI_MODE_MASTER;
//     m_spi_interface.Init.Direction = SPI_DIRECTION_1LINE;
//     m_spi_interface.Init.DataSize = SPI_DATASIZE_8BIT;
//     m_spi_interface.Init.CLKPolarity = SPI_POLARITY_LOW;
//     m_spi_interface.Init.CLKPhase = SPI_PHASE_1EDGE;
//     m_spi_interface.Init.NSS = SPI_NSS_SOFT;
//     m_spi_interface.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
//     m_spi_interface.Init.FirstBit = SPI_FIRSTBIT_MSB;
//     m_spi_interface.Init.TIMode = SPI_TIMODE_DISABLE;
//     m_spi_interface.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//     m_spi_interface.Init.CRCPolynomial = 7;
//     m_spi_interface.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
//     m_spi_interface.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

//     if (HAL_SPI_Init(&m_spi_interface) != HAL_OK) { Error_Handler(); }
  
//     __HAL_RCC_SPI2_CLK_ENABLE();
//     __HAL_RCC_GPIOB_CLK_ENABLE();

//     GPIO_InitTypeDef GPIO_InitStruct = {
//         TLC5955_SPI2_MOSI_Pin|TLC5955_SPI2_SCK_Pin,
//         GPIO_MODE_AF_PP,
//         GPIO_PULLDOWN,
//         GPIO_SPEED_FREQ_VERY_HIGH,
//         GPIO_AF1_SPI2,
//     };
    
//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//     __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB8);

// }

// void Driver::disable_spi()
// {
 
// }

// void Driver::enable_gpio_output_only()
// {
//     // disable SPI config
//     __HAL_RCC_SPI2_CLK_DISABLE();
//     HAL_GPIO_DeInit(GPIOB, TLC5955_SPI2_MOSI_Pin|TLC5955_SPI2_SCK_Pin);

//     // GPIO Ports Clock Enable 
//     __HAL_RCC_GPIOB_CLK_ENABLE();

//     // Configure GPIO pin Output Level
//     HAL_GPIO_WritePin(GPIOB, TLC5955_SPI2_LAT_Pin|TLC5955_SPI2_GSCLK_Pin|TLC5955_SPI2_MOSI_Pin|TLC5955_SPI2_SCK_Pin, GPIO_PIN_RESET);

//     // Configure GPIO pins
//     GPIO_InitTypeDef GPIO_InitStruct = {
//         TLC5955_SPI2_LAT_Pin|TLC5955_SPI2_GSCLK_Pin|TLC5955_SPI2_MOSI_Pin|TLC5955_SPI2_SCK_Pin,
//         GPIO_MODE_OUTPUT_PP,
//         GPIO_PULLDOWN,
//         GPIO_SPEED_FREQ_VERY_HIGH,
//         0
//     };

//     HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//     __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB9);
//     __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB6);
//     __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB7);
//     __HAL_SYSCFG_FASTMODEPLUS_ENABLE(SYSCFG_FASTMODEPLUS_PB8);

// }

} // namespace tlc5955