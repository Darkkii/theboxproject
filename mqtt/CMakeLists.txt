# Library name
set(LIBRARY_NAME mqtt)

# Library sources -- Add each .cpp or .c file from the folder here
add_library(${LIBRARY_NAME}
        Countdown.cpp
        IPStack.cpp)

# Include directories -- No need to edit
target_include_directories(${LIBRARY_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Libraries -- Add each library that the current module needs to this list
target_link_libraries(${LIBRARY_NAME}
        pico_stdlib
        pico_cyw43_arch_lwip_poll
)