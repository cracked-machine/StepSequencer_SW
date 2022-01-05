message("\n** Configuring STM32 Environment **")
message("** Selected Kit: ${CMAKE_C_COMPILER} **\n")

set(CMAKE_SYSTEM_NAME		Linux)
set(CMAKE_SYSTEM_VERSION	1)
set(CMAKE_SYSTEM_PROCESSOR	arm-eabi)

# binutils language settings
# compilers are set by the select kit in vscode (CMAKE_C_COMPILER/CMAKE_CXX_COMPILER)
set(CMAKE_OBJCOPY			arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP			arm-none-eabi-objdump)
set(CMAKE_SIZE			    arm-none-eabi-size)

set(TARGET              STM32G0B1KETxN)
set(ARCH			    armv6-m)
set(CORE			    cortex-m0plus)
set(ARM_ASM             mthumb)
set(LINKER_SCRIPT       ${CMAKE_SOURCE_DIR}/stm32cube_workspace/G0B1KET6N/STM32G0B1KETXN_FLASH.ld)
set(BUILD_NAME          build.elf)
set(HEX_NAME            build.hex)

# set stm32 framework property for SSD1306 lib
set(SSD1306_LIB NONE CACHE STRING "The SSD1306 STM32 library framework")
set_property(CACHE SSD1306_LIB PROPERTY STRINGS NONE LL HAL) 
set(SSD1306_LIB LL)

# set stm32 framework property for TLC5955 lib
set(TLC5955_LIB NONE CACHE STRING "The TLC5955 STM32 library framework")
set_property(CACHE TLC5955_LIB PROPERTY STRINGS NONE LL HAL) 
set(TLC5955_LIB LL)

# common build settings
set(STACK_USAGE "-fstack-usage -Wstack-usage=1024")
set(WARNING_FLAGS "-Wall -Werror -Wextra -Wdouble-promotion -Wformat=2 -Wformat-overflow -Wundef -Wformat-truncation -Wfloat-equal -Wshadow")
set(COMMON_FLAGS "-march=${ARCH} -mcpu=${CORE} -${ARM_ASM} ${STACK_USAGE} ${WARNING_FLAGS} ${OPTIM_LVL} ${DEBUG_LVL} ${ADDITIONAL_FLAGS} -mfloat-abi=soft -pedantic -Wall -Wextra -Wfloat-equal -Wshadow -Wall -Wl,--gc-sections -fmessage-length=0 -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")
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

# add the stm32 framework compiler definition for SSD1306 lib
if(SSD1306_LIB STREQUAL HAL)
    add_compile_definitions(${TARGET} USE_HAL_DRIVER)
    add_compile_definitions(${TARGET} USE_SSD1306_HAL_DRIVER)
elseif(SSD1306_LIB STREQUAL LL)
    add_compile_definitions(${TARGET} USE_FULL_LL_DRIVER)
    add_compile_definitions(${TARGET} USE_SSD1306_LL_DRIVER)
elseif(SSD1306_LIB STREQUAL NONE)    
    add_compile_definitions(${TARGET} USE_SSD1306_BAREMETAL)
endif()

# add the stm32 framework compiler definition for TLC5955 lib
if(TLC5955_LIB STREQUAL HAL)
    add_compile_definitions(${TARGET} USE_HAL_DRIVER)
    add_compile_definitions(${TARGET} USE_TLC5955_HAL_DRIVER)
elseif(TLC5955_LIB STREQUAL LL)
    add_compile_definitions(${TARGET} USE_FULL_LL_DRIVER)
    add_compile_definitions(${TARGET} USE_TLC5955_LL_DRIVER)
elseif(TLC5955_LIB STREQUAL LL)
    add_compile_definitions(${TARGET} USE_TLC5955_BAREMETAL)
endif()

add_compile_definitions(
    ${TARGET} STM32G0B1xx
)



message("CMAKE_CXX_FLAGS_DEBUG:" ${CMAKE_CXX_FLAGS_DEBUG})
message("CMAKE_CXX_FLAGS_RELEASE:" ${CMAKE_CXX_FLAGS_RELEASE})
message("CMAKE_CXX_FLAGS_MINSIZEREL:" ${CMAKE_CXX_FLAGS_MINSIZEREL})
message("CMAKE_CXX_FLAGS_RELWITHDEBINFO:" ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

