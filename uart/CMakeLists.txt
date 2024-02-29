# Library -- Add each .cpp or .c file from the folder here
add_library(Uart
        PicoUart.cpp 
        RingBuffer.cpp)

target_include_directories(Uart INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Uart
        pico_stdlib
        hardware_gpio
)