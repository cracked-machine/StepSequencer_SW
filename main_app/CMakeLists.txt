target_sources(${BUILD_NAME} PRIVATE
    src/mainapp.cpp
    src/led_manager.cpp
    src/sequence_manager.cpp
    src/keypad_manager.cpp
    src/display_manager.cpp
    src/step.cpp
)

target_include_directories(${BUILD_NAME} PRIVATE inc)