#include <tlc5955.hpp>
#include <iostream>

int main()
{
    tlc5955::Driver leds;
    std::cout << leds.startup_tests() << std::endl;
    return 0;
}