#include <stdio.h>
#include <iostream>
#include <string>
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
    char payload[256] = { 0 };
    strncpy(payload, (char *)message.payload, message.payloadlen);
    string payloadString{ payload };
    cout << payloadString;
    SettingsMessage settingsMessage(payloadString);


    // mqttHandler->notifyObservers();

    // printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
    //        message.qos, message.retained, message.dup, message.id);
    // printf("On topic %.*s\n", md.topicName.lenstring.len, md.topicName.lenstring.data);
    // printf("Payload %.*s\n", message.payloadlen, (char *)message.payload);
    cout << settingsMessage.getString() << endl;
    // cout << settingsMessage.getAuto() << endl;
}