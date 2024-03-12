#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include "MQTTClient.h"
#include "MQTTHandler.h"
#include "SettingsMessage.h"
#include "nlohmann/json.hpp"

using namespace std;

#define USE_MQTT

void messageHandler(MQTT::MessageData &md);
// static global pointer to allow mqtt callback function to access public member functions
static shared_ptr<MQTTHandler> mqttHandler;

int main()
{
    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");
    mqttHandler = make_shared<MQTTHandler>(messageHandler);

#ifdef USE_MQTT
    mqttHandler->connect();
    auto mqttTimeout = make_timeout_time_ms(5000);
#endif

    while (true)
    {

#ifdef USE_MQTT
        if (time_reached(mqttTimeout))
        {
            mqttHandler->update();
            mqttTimeout = make_timeout_time_ms(5000);
        }

        mqttHandler->keepAlive();
#endif

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