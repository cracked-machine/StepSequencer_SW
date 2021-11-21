

set(CMAKE_SYSTEM_NAME		Linux)
set(CMAKE_SYSTEM_VERSION	1)
set(CMAKE_SYSTEM_PROCESSOR	arm-eabi)


# # toolchain language settings
set(CMAKE_C_COMPILER		arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER		arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER		arm-none-eabi-gcc)

set(TARGET              STM32G0B1KETxN)
set(ARCH			    armv6-m)
set(CORE			    cortex-m0plus)
set(ARM_ASM             mthumb)
set(LINKER_SCRIPT       ${CMAKE_SOURCE_DIR}/STM32G0B1KETXN_FLASH.ld)
set(BUILD_NAME            build.elf)
set(HEX_NAME            build.hex)

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

# common defines
add_compile_definitions(${TARGET} USE_HAL_DRIVER)
add_compile_definitions(${TARGET} STM32G0B1xx)
add_compile_definitions(${TARGET} SSD1306_USE_SPI)
add_compile_definitions(${TARGET} DEBUG)

