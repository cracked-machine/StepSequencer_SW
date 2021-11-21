target_sources(${BUILD_NAME} PRIVATE
    src/SEGGER_RTT_printf.c
    src/SEGGER_RTT.c
)

target_include_directories(${BUILD_NAME} PRIVATE inc)