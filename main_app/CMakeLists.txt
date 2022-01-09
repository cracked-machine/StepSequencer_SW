target_sources(${BUILD_NAME} PRIVATE
    src/mainapp.cpp
    src/LedManager.cpp
   
)

target_include_directories(${BUILD_NAME} PRIVATE inc)