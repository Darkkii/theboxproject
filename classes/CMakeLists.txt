# Class Libraries -- Add .cpp file from each class here
add_library(Classes ExampleClass.cpp RotSW.cpp RotSW.h)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

target_link_libraries(Classes
        pico_stdlib
        hardware_pwm
        hardware_gpio
        hardware_i2c
)