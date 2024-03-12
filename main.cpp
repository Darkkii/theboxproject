#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "MQTTHandler.h"
#include "SettingsMessage.h"
#include "nlohmann/json.hpp"

using namespace std;

#define USE_MQTT
#define STOP_BITS 1 // for simulator
//#define STOP_BITS 2 // for real system

void messageHandler(MQTT::MessageData &md);
// // static global pointer to allow mqtt callback function to access public member functions
static shared_ptr<MQTTHandler> mqttHandler;

int main()
{
    // Initialize chosen serial port
    stdio_init_all();
    printf("\nBoot\n");

    mqttHandler = make_shared<MQTTHandler>(messageHandler);
    // mqttHandler->connect("PICOQ5-9k195", "Q5-9k195", "192.168.137.1");
    mqttHandler->connect();
    auto mqttTimeout = make_timeout_time_ms(5000);

    while (true)
    {
        if (time_reached(mqttTimeout))
        {
            mqttHandler->update();
            mqttTimeout = make_timeout_time_ms(5000);
        }

        mqttHandler->keepAlive();
    }
}

void messageHandler(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;
    char payload[256];
    strncpy(payload, (char *)message.payload, message.payloadlen);
    string payloadString{ payload };
    istringstream stream{ payloadString };
    bool mode = true;
    int setpoint = 0;
    stream.ignore(256, ' ');
    stream >> boolalpha >> mode;
    stream.ignore(256, ' ');
    stream.ignore(256, ' ');
    stream >> setpoint;

    SettingsMessage settingsMessage(mode, setpoint);

    mqttHandler->notifyObservers();
    StatusMessage msg(2, 1, 3, false, false, 2, 3, 30);
    mqttHandler->send(msg);
}