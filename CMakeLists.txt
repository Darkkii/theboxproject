# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set board type because we are building for PicoW
set(PICO_BOARD pico_w)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(pico-modbus C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set CMP policy to use the NEW format to remove errors when using CMake v3.24+
if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
	set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

#add_compile_definitions(MQTTCLIENT_QOS2=1)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
file(GLOB MQTT_SOURCES "paho.mqtt.embedded-c/MQTTPacket/src/*.c")

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
        main.cpp
        ${MQTT_SOURCES}

)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Included directories
target_include_directories(${PROJECT_NAME} PRIVATE
        "${PROJECT_BINARY_DIR}"
        paho.mqtt.embedded-c/MQTTPacket/src
        paho.mqtt.embedded-c/MQTTClient/src
)

# Subdirectories
add_subdirectory(display)
add_subdirectory(modbus)
add_subdirectory(mqtt)
add_subdirectory(uart)
add_subdirectory(sensors)
add_subdirectory(i2c)
add_subdirectory(misc)
add_subdirectory(observer)

# JSON parser
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PRIVATE
        pico_stdlib
        display
        modbus
        mqtt
        uart
        misc
        sensors
        i2c
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)