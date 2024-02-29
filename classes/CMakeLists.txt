# Library -- Add each .cpp or .c file from the folder here
add_library(Classes
            GMP252.cpp)

target_include_directories(Classes INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)