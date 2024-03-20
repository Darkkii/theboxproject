#include "Eeprom.h"
#include "GMP252.h"
#include "HMP60.h"
#include "I2CHandler.h"
#include "MIO12V.h"
#include "MQTTClient.h"
#include "MQTTHandler.h"
#include "ModbusClient.h"
#include "PicoSW.h"
#include "SDP600.h"
#include "SettingsMessage.h"
#include "State.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "uart/PicoUart.h"

#include <iostream>
#include <memory>

using namespace std;

#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define MODBUS_BAUD_RATE 9600
#define DEFAULT_NETWORK_ID "KME662"      // Default Wi-Fi SSID
#define DEFAULT_NETWORK_PW "SmartIot"    // Default Wi-Fi password
#define DEFAULT_BROKER_IP "192.168.1.10" // Default MQTT Broker IP address

#define STOP_BITS 1 // for simulator
// #define STOP_BITS 2 // for real system

void messageHandler(MQTT::MessageData &md);

// Global shared pointer to allow MQTT callback function access to member
// functions.
static shared_ptr<MQTTHandler> mqttHandler;

int main()
{
    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");

    // Initialize all classes, add observer/subject relations.
    mqttHandler = make_shared<MQTTHandler>(messageHandler);
    auto i2cHandler{ make_shared<I2CHandler>() };
    auto uart{ std::make_shared<PicoUart>(
        UART_NR, UART_TX_PIN, UART_RX_PIN, MODBUS_BAUD_RATE, STOP_BITS) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };
    auto fanController{ make_shared<MIO12V>(rtu_client) };
    auto gmp252{ make_shared<GMP252>(rtu_client) };
    auto hmp252{ make_shared<HMP60>(rtu_client) };
    auto sdp600{ make_shared<SDP600>(i2cHandler->getI2CBus(1)) };
    auto eeprom{ make_shared<Eeprom>(i2cHandler) };
    auto state{ make_shared<State>(i2cHandler,
                                   gmp252,
                                   hmp252,
                                   fanController,
                                   sdp600,
                                   mqttHandler,
                                   eeprom) };
    fanController->addObserver(state);
    mqttHandler->addObserver(state);

    PicoSW picoSW(true, true, true, true, true);
    PicoSW_event swEvent;

    // Try to connect to the saved network settings, in case of failure, connect
    // to default network.
    if (!state->ConnectMQTT(eeprom->read(EEPROM_REG_NETWORK_ID),
                            eeprom->read(EEPROM_REG_NETWORK_PW),
                            eeprom->read(EEPROM_REG_BROKER_IP)))
    {
        state->ConnectMQTT(
            DEFAULT_NETWORK_ID, DEFAULT_NETWORK_PW, DEFAULT_BROKER_IP);
    }

    auto passiveUpdateInterval = make_timeout_time_ms(0);

    while (true)
    {
        if (time_reached(passiveUpdateInterval))
        {
            passiveUpdateInterval = make_timeout_time_ms(1000);
            gmp252->update();
            hmp252->update();
            sdp600->update();
            state->updateMQTT();
            state->update();
        }
        mqttHandler->keepAlive();

        while ((swEvent = picoSW.getEvent()) != NO_EVENT)
        {
            switch (swEvent)
            {
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

// Callback function to handle received MQTT messages.
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