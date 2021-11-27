


#include <catch2/catch_all.hpp>
#include <iostream>
#include <tlc5955.hpp>

TEST_CASE("TestTLC5955 part 2", "[cpp_tlc5955]")
{

    tlc5955::Driver leds;
    REQUIRE_FALSE(leds.startup_tests());
    
}