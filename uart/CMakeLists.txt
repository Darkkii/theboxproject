# Library name
set(LIBRARY_NAME uart)

# Library sources -- Add each .cpp or .c file from the folder here
add_library(${LIBRARY_NAME}
        PicoUart.cpp 
        RingBuffer.cpp)

# Include directories -- No need to edit
target_include_directories(${LIBRARY_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Libraries -- Add each library that the current module needs to this list
target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        hardware_uart
        hardware_gpio
)