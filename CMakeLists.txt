cmake_minimum_required(VERSION 3.13)

# recommended compiler: gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads


# variables
set(TARGET              STM32G0B1KETxN)
set(ARCH			    armv6-m)
set(CORE			    cortex-m0plus)
set(ARM_ASM             mthumb)
set(LINKER_SCRIPT       ${CMAKE_SOURCE_DIR}/STM32G0B1KETXN_FLASH.ld)
set(ELF_NAME            build.elf)
set(HEX_NAME            build.hex)
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(DEBUG_LVL           "-g3")
set(OPTIM_LVL           "-O0")

# common defines
add_compile_definitions(${TARGET} USE_HAL_DRIVER)
add_compile_definitions(${TARGET} STM32G0B1xx)
add_compile_definitions(${TARGET} SSD1306_USE_SPI)
add_compile_definitions(${TARGET} DEBUG)

# common build settings
set(COMMON_FLAGS "-march=${ARCH} -mcpu=${CORE} -${ARM_ASM} ${OPTIM_LVL} ${DEBUG_LVL} ${ADDITIONAL_FLAGS} -mfloat-abi=soft -pedantic -Wall -Wextra -Wfloat-equal -Wshadow -Wall -Wl,--gc-sections -fmessage-length=0 -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")
set(CMAKE_EXE_LINKER_FLAGS  "-mthumb -mcpu=cortex-m0 -specs=nosys.specs -static -Wl,-Map=build.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group -T${LINKER_SCRIPT}" CACHE INTERNAL "exe link flags")

# C compiler settings
set(C_FLAGS "-ffunction-sections -fdata-sections -g3 -fstack-usage -Wno-unused-parameter -Wno-pedantic --specs=nano.specs")
set(CMAKE_C_FLAGS           "${COMMON_FLAGS} ${C_FLAGS}" CACHE INTERNAL "c compiler flags")
enable_language(C)

# c++ compiler settings
set(CPP_FLAGS "-fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics -ftemplate-backtrace-limit=0")
set(CMAKE_CXX_FLAGS	        "${COMMON_FLAGS} ${CPP_FLAGS}" CACHE INTERNAL "cpp compiler flags")
enable_language(CXX)

## Assembler compiler settings
set(CMAKE_ASM_FLAGS	        "${COMMON_FLAGS} -MP -MD -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
enable_language(ASM)

# The project name
project(STEP_SEQUENCER VERSION 0.1 DESCRIPTION "STEP_SEQUENCER")

# toolchain language settings
set(CMAKE_C_COMPILER		arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER		arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER		arm-none-eabi-gcc)

# include dependencies
include_directories(
    "source/cube_gen_src/Inc"
    "source/hal_drivers/STM32G0xx_HAL_Driver/Inc"
    "source/hal_drivers/STM32G0xx_HAL_Driver/Inc/Legacy"
    "source/hal_drivers/CMSIS/Device/ST/STM32G0xx/Include"
    "source/hal_drivers/CMSIS/Include"
    "source/main_app"
    "source/cpp_tlc5955/"
    "source/cpp_ssd1306/"
    "source/segger_rtt"
)

# source files
file(GLOB_RECURSE 
	 MAIN_SOURCE
     "source/cube_gen_src/Src/*.c"
     "source/hal_drivers/STM32G0xx_HAL_Driver/Src/*.c"
     "source/main_app/*.cpp"
     "source/cpp_ssd1306/*.cpp"
     "source/cpp_tlc5955/*.cpp"
     "source/cube_gen_src/Startup/startup_stm32g0b1ketxn.s"
     "source/segger_rtt/*.c"
)

# build the elf binary
add_executable(${ELF_NAME} ${MAIN_SOURCE})

# display size info
add_custom_target(size ALL arm-none-eabi-size ${ELF_NAME} DEPENDS ${ELF_NAME})

# objcopy the elf file as a hex file
add_custom_target(build.bin ALL DEPENDS ${ELF_NAME} COMMAND arm-none-eabi-objcopy -O ihex ${ELF_NAME} ${HEX_NAME})
