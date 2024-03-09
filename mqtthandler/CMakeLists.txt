# Library name
set(LIBRARY_NAME mqtthandler)

# Library sources -- Add each .cpp or .c file from the folder here
add_library(${LIBRARY_NAME}
        MQTTHandler.cpp
)

# Include directories -- No need to edit
target_include_directories(${LIBRARY_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../paho.mqtt.embedded-c/MQTTPacket/src
        ../paho.mqtt.embedded-c/MQTTClient/src
)

# Link Libraries -- Add each library that the current module needs to this list
target_link_libraries(${LIBRARY_NAME}
        PRIVATE
        pico_stdlib
        mqtt
        observer
)