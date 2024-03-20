# Library name
set(LIBRARY_NAME classes)

# Library sources -- Add each .cpp or .c file from the folder here
add_library(${LIBRARY_NAME}
        MIO12V.cpp
        PicoSW.cpp
        State.cpp
        State.h
)

# Include directories -- No need to edit
target_include_directories(${LIBRARY_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Libraries -- Add each library that the current module needs to this list
target_link_libraries(${LIBRARY_NAME}
        PRIVATE
        pico_stdlib
        modbus
        observer
        display
        i2c
        sensors
        mqtt
)