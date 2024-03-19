#include <iostream>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"
#include <memory>

#include "MQTTClient.h"
#include "ModbusClient.h"
#include "GMP252.h"
#include "HMP60.h"
#include "SDP600.h"
#include "MIO12V.h"
#include "PicoSW.h"
#include "I2CHandler.h"
#include "State.h"
#include "Eeprom.h"
#include "MQTTHandler.h"
#include "SettingsMessage.h"
#include "nlohmann/json.hpp"

#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define MODBUS_BAUD_RATE 9600

#if 1
#define DEFAULT_NETWORK_ID "SmartIotMQTT"
#define DEFAULT_NETWORK_PW "SmartIot"
#define DEFAULT_BROKER_IP  "192.168.1.10"
#else
#define DEFAULT_NETWORK_ID "abc"
#define DEFAULT_NETWORK_PW "11111111"
#define DEFAULT_BROKER_IP  "192.168.137.1"
#endif

#define STOP_BITS 1 // for simulator
//#define STOP_BITS 2 // for real system

void messageHandler(MQTT::MessageData &md);

// static global pointer to allow mqtt callback function to access public member functions
static shared_ptr<MQTTHandler> mqttHandler;

int main()
{
    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");
    mqttHandler = make_shared<MQTTHandler>(messageHandler);

    auto i2cHandler{ make_shared<I2CHandler>() };

    auto modbus_poll = make_timeout_time_ms(3000);
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, MODBUS_BAUD_RATE, STOP_BITS) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };

    auto fanController{ make_shared<MIO12V>(rtu_client) };
    auto gmp252{ make_shared<GMP252>(rtu_client) };
    auto hmp252{ make_shared<HMP60>(rtu_client) };
    auto sdp600{ make_shared<SDP600>(i2cHandler->getI2CBus(1)) };

    auto eeprom{ make_shared<Eeprom>(i2cHandler) };

    auto state{ make_shared<State>(i2cHandler, gmp252, hmp252, fanController, sdp600, mqttHandler, eeprom) };

    fanController->addObserver(state);
    mqttHandler->addObserver(state);

    PicoSW picoSW(true, true, true, true, true);
    PicoSW_event swEvent;

    if (!state->ConnectMQTT(eeprom->read(EEPROM_REG_NETWORK_ID),
                            eeprom->read(EEPROM_REG_NETWORK_PW),
                            eeprom->read(EEPROM_REG_BROKER_IP))) {
        state->ConnectMQTT(DEFAULT_NETWORK_ID,
                           DEFAULT_NETWORK_PW,
                           DEFAULT_BROKER_IP);
    }

    auto passiveUpdateInterval = make_timeout_time_ms(0);

    while (true) {
        if (time_reached(passiveUpdateInterval)) {
            passiveUpdateInterval = make_timeout_time_ms(1000);
            gmp252->update();
            hmp252->update();
            sdp600->update();
            state->updateMQTT();
            state->update();
        }
        mqttHandler->keepAlive();

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
                    state->toggleScreen();
                    break;
                case NO_EVENT:
                    break;
            }
        }
        state->adjustFan();
        sleep_ms(50);
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

    mqttHandler->setSettingsMessage(SettingsMessage(mode, setpoint));
    mqttHandler->notifyObservers();
}