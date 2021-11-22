# build STM32 HAL as static lib. 
# includes must be PUBLIC because they are used when building dependents
add_library(STM32Cube STATIC "")
target_sources(STM32Cube PRIVATE
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_cortex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_dma.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_exti.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_flash.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_rcc.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_spi_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_spi.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim_ex.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_tim.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_gpio.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.c
G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c)
target_include_directories(STM32Cube PUBLIC 
    G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Inc
    G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
    G0B1KET6N/Drivers/CMSIS/Include
    G0B1KET6N/Drivers/CMSIS/Device/ST/STM32G0xx/Include/
    G0B1KET6N/Core/Inc
)

# link Core files with ${BUILD_NAME} target executable
target_sources(${BUILD_NAME} PRIVATE 
    G0B1KET6N/Core/Startup/startup_stm32g0b1ketxn.s
    G0B1KET6N/Core/Src/gpio.c
    G0B1KET6N/Core/Src/main.c
    G0B1KET6N/Core/Src/spi.c
    G0B1KET6N/Core/Src/stm32g0xx_hal_msp.c
    G0B1KET6N/Core/Src/stm32g0xx_it.c
    G0B1KET6N/Core/Src/syscalls.c
    G0B1KET6N/Core/Src/sysmem.c
    G0B1KET6N/Core/Src/system_stm32g0xx.c)
target_include_directories(${BUILD_NAME} PRIVATE 
    G0B1KET6N/Core/Inc
)