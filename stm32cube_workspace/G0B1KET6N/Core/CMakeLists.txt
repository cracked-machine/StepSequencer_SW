target_sources(${BUILD_NAME} PRIVATE
    Startup/startup_stm32g0b1ketxn.s
    Src/gpio.c
    Src/main.c
    Src/spi.c
    Src/stm32g0xx_hal_msp.c
    Src/stm32g0xx_it.c
    Src/syscalls.c
    Src/sysmem.c
    Src/system_stm32g0xx.c
)

target_include_directories(${BUILD_NAME} PRIVATE 
    Inc
)
