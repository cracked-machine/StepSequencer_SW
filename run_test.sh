#!/bin/sh
set -e

# build for STM32 
echo "\n====================="
echo "Building for STM32..."
echo "=====================\n"
rm build/CMakeCache.txt
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=arm-none-eabi-gcc-9.3.1 -H. -B./build -G Ninja
cmake --build build --config Debug --clean-first --target all -j 10 --


# build for x86_64 and run CTest
echo "\n======================"
echo "Building for x86_64..."
echo "======================\n"
rm build/CMakeCache.txt
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=x86_64-linux-gnu-gcc-9 -DCMAKE_CXX_COMPILER:FILEPATH=x86_64-linux-gnu-g++-9 -H. -B./build -G Ninja
cmake --build build --config Debug --clean-first --target build -j 10 --

echo "\n======================"
echo "Running CTest tests..."
echo "======================\n"
cd build
ctest -j10 -C Debug -T test --output-on-failure
cd ..


