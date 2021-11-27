
#include <catch2/catch_all.hpp>
#include <ssd1306.hpp>

TEST_CASE("Test Fonts", "[ssd1306_fonts]")
{

    SECTION("3x5Font")
    {
        static ssd1306::Font3x5 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 5);
        REQUIRE(test_font.size() == 475);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(200, result));
        REQUIRE(result ==  20480);       
    }

    SECTION("5x7Font")
    {
        static ssd1306::Font5x7 test_font;
        REQUIRE(test_font.width() == 5);
        REQUIRE(test_font.height() == 7);
        REQUIRE(test_font.size() == 680);   
        uint32_t result{0};     
        REQUIRE(test_font.get_pixel(200, result));
        REQUIRE(result ==  20480);
    }    

    SECTION("7x10Font")
    {
        static ssd1306::Font7x10 test_font;
        REQUIRE(test_font.width() == 7);
        REQUIRE(test_font.height() == 10);
        REQUIRE(test_font.size() == 950);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(200, result));
        REQUIRE(result ==  2048);
    }

    SECTION("11x16Font")
    {
        static ssd1306::Font11x16 test_font;
        REQUIRE(test_font.width() == 11);
        REQUIRE(test_font.height() == 16);
        REQUIRE(test_font.size() == 1710);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(179, result));
        REQUIRE(result ==  8192);
    }

    SECTION("16x26Font")
    {
        static ssd1306::Font16x26 test_font;
        REQUIRE(test_font.width() == 16);
        REQUIRE(test_font.height() == 26);
        REQUIRE(test_font.size() == 2470);
        uint32_t result{0};
        REQUIRE(test_font.get_pixel(150, result));
        REQUIRE(result ==  57407);
    }

}

TEST_CASE("Test Display", "[ssd1306_display]")
{
    static ssd1306::Font16x26 font;
    static ssd1306::Display oled;
    REQUIRE(oled.init());
    

    
    SECTION("Write white on black with padding == 1")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font, 2, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 1, true));
    }
    SECTION("Write white on black with padding == 1")
    {
        std::stringstream text("TEST");
        REQUIRE(oled.write(text, font, 2, 2, ssd1306::Colour::White, ssd1306::Colour::Black, 1, true));
    }
    SECTION("Write with invalid x position")
    {
        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font, 255, 2, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true));
    }
    SECTION("Write with invalid y position")
    {
        std::stringstream text("TEST");
        REQUIRE_FALSE(oled.write(text, font, 2, 255, ssd1306::Colour::Black, ssd1306::Colour::White, 3, true));
    }
    SECTION("Write invalid char")
    {
        REQUIRE_FALSE(oled.write_char(0, font, ssd1306::Colour::Black, 3));
    }

    SECTION("Write invalid string")
    {
        std::stringstream text;
        REQUIRE_FALSE(oled.write_string(text, font, ssd1306::Colour::White, 3));
    }
    
    
    oled.print_buffer_stdout();
}