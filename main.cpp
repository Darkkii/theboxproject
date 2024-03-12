#include <stdio.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"
#include <memory>

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "SDP600.h"
#include "MIO12V.h"
#include "PicoSW.h"
#include "I2CHandler.h"
#include "State.h"

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#if 0
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif
#include "MQTTHandler.h"
#include "SettingsMessage.h"
#include "nlohmann/json.hpp"

#define BAUD_RATE 9600

#define USE_MQTT
#define STOP_BITS 1 // for simulator
//#define STOP_BITS 2 // for real system

void messageHandler(MQTT::MessageData &md);
// static global pointer to allow mqtt callback function to access public member functions
static shared_ptr<MQTTHandler> mqttHandler;

int main()
{
    stdio_init_all();
    printf("\nBoot\n");
    mqttHandler = make_shared<MQTTHandler>(messageHandler);

    auto i2cHandler {make_shared<I2CHandler>()};

    auto modbus_poll = make_timeout_time_ms(3000);
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };

    auto fanController {make_shared<MIO12V>(rtu_client)};
    auto gmp252 {make_shared<GMP252>(rtu_client)};
    auto hmp252 {make_shared<HMP60>(rtu_client)};
    auto sdp600 {make_shared<SDP600>(i2cHandler->getI2CBus(1))};

    auto state {make_shared<State>(i2cHandler, gmp252, hmp252, fanController, sdp600, mqttHandler)};

    fanController->addObserver(state);

    PicoSW picoSW(true, true, true, true, true);
    PicoSW_event swEvent;

#ifdef USE_MQTT
    mqttHandler->connect();
    auto mqttTimeout = make_timeout_time_ms(5000);
#endif

    while (true) {
#ifdef USE_MQTT
        if (time_reached(mqttTimeout))
        {
            mqttHandler->update();
            mqttTimeout = make_timeout_time_ms(5000);
        }

        mqttHandler->keepAlive();
#endif
        if (time_reached(modbus_poll)) {
            modbus_poll = delayed_by_ms(modbus_poll, 3000);
            gmp252->update();
            hmp252->update();
            sdp600->update();
            state->update();
        }

        while ((swEvent = picoSW.getEvent()) != NO_EVENT) {
            switch (swEvent) {
                case CLOCKWISE:
                    state->clockwise();
                    break;
                case COUNTER_CLOCKWISE:
                    state->counter_clockwise();
                    break;
                case ROT_PRESS:
                    state->setTarget();
                    break;
                case SW_0_PRESS:
                    state->toggleMode();
                    break;
                case SW_1_PRESS:
                    state->backspace();
                    break;
                case SW_2_PRESS:
                    state->toggle_MQTT_input();
                    break;
            }
        }

        state->adjustFan();

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