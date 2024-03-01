# Class Libraries -- Add .cpp file from each class here
add_library(Classes ExampleClass.cpp RotSW.cpp RotSW.h)

target_link_libraries(Classes
        pico_stdlib
)