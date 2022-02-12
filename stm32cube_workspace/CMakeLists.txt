# build STM32 LL as static lib. 
# includes must be PUBLIC because they are used when building dependents
   
    add_library(STM32CubeLL STATIC "")
    target_sources(STM32CubeLL PUBLIC
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_crs.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_dma.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_exti.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_gpio.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_i2c.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_pwr.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_rcc.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_spi.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_tim.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_utils.c
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_ll_usart.c
    )
    target_include_directories(STM32CubeLL PUBLIC 
    
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Inc
        G0B1KET6N/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
        G0B1KET6N/Drivers/CMSIS/Include
        G0B1KET6N/Drivers/CMSIS/Device/ST/STM32G0xx/Include/
        G0B1KET6N/Core/Inc)       

    # link Core files with ${BUILD_NAME} target executable
    target_sources(${BUILD_NAME} PUBLIC 
        G0B1KET6N/Core/Startup/startup_stm32g0b1ketxn.s
        G0B1KET6N/Core/Src/system_stm32g0xx.c
        G0B1KET6N/Core/Src/main.c
        G0B1KET6N/Core/Src/gpio.c
        G0B1KET6N/Core/Src/i2c.c
        G0B1KET6N/Core/Src/dma.c
        G0B1KET6N/Core/Src/spi.c
        G0B1KET6N/Core/Src/tim.c
        G0B1KET6N/Core/Src/usart.c
        G0B1KET6N/Core/Src/stm32g0xx_it.c
        
    )
    target_include_directories(${BUILD_NAME} PUBLIC 
        G0B1KET6N/Core/Inc
    )


