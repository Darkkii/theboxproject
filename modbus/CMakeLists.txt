# Library -- Add each .cpp or .c file from the folder here
add_library(Modbus
        ModbusClient.cpp
        ModbusRegister.cpp)

target_include_directories(Modbus INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/uart
)

target_link_libraries(Modbus
        pico
        Uart
)