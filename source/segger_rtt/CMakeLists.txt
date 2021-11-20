target_sources(${BUILD_NAME} PRIVATE
    SEGGER_RTT_printf.c
    SEGGER_RTT.c
)

target_include_directories(${BUILD_NAME} PRIVATE .)