#include <stdio.h>
#include "MQTTHandler.h"

using namespace std;

#define USE_MQTT

int main()
{
    MQTTHandler mqttHandler;

    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");

#ifdef USE_MQTT
    mqttHandler.connect();
    auto mqttTimeout = make_timeout_time_ms(5000);
#endif

    while (true)
    {

#ifdef USE_MQTT
        if (time_reached(mqttTimeout))
        {
            mqttHandler.update();
            mqttTimeout = make_timeout_time_ms(5000);
        }

        mqttHandler.keepAlive();
#endif

    }
}

