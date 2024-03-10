#include <stdio.h>
#include <memory>
#include "MQTTClient.h"
#include "MQTTHandler.h"

using namespace std;

#define USE_MQTT

void messageHandler(MQTT::MessageData &md);
// static global to allow callback access to public member functions
static auto mqttHandler{ make_shared<MQTTHandler>(messageHandler) };

int main()
{
    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");


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

    mqttHandler->notifyObservers();

    // printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
    //        message.qos, message.retained, message.dup, message.id);
    // printf("On topic %.*s\n", md.topicName.lenstring.len, md.topicName.lenstring.data);
    // printf("Payload %.*s\n", (int)message.payloadlen, (char *)message.payload);
}