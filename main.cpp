// #define MQTT_DEBUG
#include <stdio.h>
// #include "IPStack.h"
// #include "Countdown.h"
// #include "MQTTClient.h"
#include "MQTTHandler.h"


#define USE_MQTT


int main()
{
    MQTTHandler mqttHandler;

    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");

#ifdef USE_MQTT
    mqttHandler.connect();
    auto mqttTimeout = make_timeout_time_ms(10000);
#endif

    while (true)
    {

#ifdef USE_MQTT
        // if (time_reached(mqttTimeout))
        // {
        //     mqttHandler.update();
        //     mqttTimeout = make_timeout_time_ms(10000);
        // }

        mqttHandler.keepAlive();
#endif

    }
}

