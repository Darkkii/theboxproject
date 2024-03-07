#include <stdio.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

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

#define BAUD_RATE 9600

//#define USE_MODBUS
//#define USE_MQTT
#define USE_SSD1306
#define TEST_SENSORS
#define TEST_FAN_MOTOR
#define TEST_SW
#define TEST_PRESSURE_SENSOR


#ifdef USE_SSD1306
static const uint8_t raspberry26x32[] =
{ 0x0, 0x0, 0xe, 0x7e, 0xfe, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xfc, 0xfe,
 0xfe, 0xff, 0xff,0xff, 0xff, 0xff, 0xfe, 0x7e,
 0x1e, 0x0, 0x0, 0x0, 0x80, 0xe0, 0xf8, 0xfd,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd,
 0xf8, 0xe0, 0x80, 0x0, 0x0, 0x1e, 0x7f, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0x7f, 0x1e, 0x0, 0x0,
 0x0, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x3f, 0x3f,
 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f,
 0x3f, 0x1f, 0x1f, 0xf, 0x7, 0x3, 0x0, 0x0 };
#endif // USE_SSD1306

#ifdef USE_MQTT
void messageArrived(MQTT::MessageData &md)
{
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}

static const char *topic = "test-topic";
#endif // USE_MQTT

using namespace std;

union pressure_conversion {
    uint16_t u16;
    int16_t _int;
};

int main()
{

    // Initialize chosen serial port
    stdio_init_all();
    printf("\nBoot\n");

    const uint led_pin = 22;
    const uint button = 9;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    auto modbus_poll = make_timeout_time_ms(3000);
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };

    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);

    uint16_t targetFanSpeed = 0; // get from eeprom ?
    int16_t targetPressure = 0; // get from eeprom ?
    bool manual = true; // get from eeprom ?
    bool adjust = false; // initially false I think

    MIO12V fanController(rtu_client);
    sleep_ms(100);
    fanController.setFanSpeed(targetFanSpeed);
    printf("Fan Speed: %u\n", targetFanSpeed);

    SDP600 sdp600{};
    GMP252 gmp252{ rtu_client };
    HMP60 hmp60{ rtu_client };
    gmp252.update();
    sleep_ms(5);
    gmp252.update();
    sleep_ms(5);
    hmp60.update();
    sleep_ms(5);
    PicoSW picoSW(true, true, true);
    PicoSW_event swEvent;
#ifdef USE_SSD1306
    ssd1306 display(i2c1);
#endif // USE_SSD1306

#ifdef USE_MQTT
    //IPStack ipstack("SSID", "PASSWORD"); // example
    IPStack ipstack("SmartIotMQTT", "SmartIot"); // example
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    int rc = ipstack.connect("192.168.1.10", 1883);
    if (rc != 1) {
        printf("rc from TCP connect is %d\n", rc);
    }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"PicoW-sample";
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        while (true) {
            tight_loop_contents();
        }
    }
    printf("MQTT connected\n");

    // We subscribe QoS2. Messages sent with lower QoS will be delivered using the QoS they were sent with
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;
#endif // USE_MQTT

    while (true) {
#ifdef USE_MQTT
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 2000);
            if (!client.isConnected()) {
                printf("Not connected...\n");
                rc = client.connect(data);
                if (rc != 0) {
                    printf("rc from MQTT connect is %d\n", rc);
                }

            }
            char buf[100];
            int rc = 0;
            MQTT::Message message;
            message.retained = false;
            message.dup = false;
            message.payload = (void *)buf;
            switch (mqtt_qos) {
                case 0:
                    // Send and receive QoS 0 message
                    sprintf(buf, "Msg nr: %d QoS 0 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS0;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
                case 1:
                    // Send and receive QoS 1 message
                    sprintf(buf, "Msg nr: %d QoS 1 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS1;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
#if MQTTCLIENT_QOS2
                case 2:
                    // Send and receive QoS 2 message
                    sprintf(buf, "Msg nr: %d QoS 2 message", ++msg_count);
                    printf("%s\n", buf);
                    message.qos = MQTT::QOS2;
                    message.payloadlen = strlen(buf) + 1;
                    rc = client.publish(topic, message);
                    printf("Publish rc=%d\n", rc);
                    ++mqtt_qos;
                    break;
#endif // MQTTCLIENT_QOS2
                default:
                    mqtt_qos = 0;
                    break;
            }
        }

        cyw43_arch_poll(); // obsolete? - see below
        client.yield(100); // socket that client uses calls cyw43_arch_poll()
#endif // USE_MQTT
        if (time_reached(modbus_poll)) {

            display.fill(0);

            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            modbus_poll = delayed_by_ms(modbus_poll, 3000);

            char CO2[17];
            float co2 = gmp252.getCO2();
            snprintf(CO2, 17, "%4s: %c%-5.0f %s", "CO2", ' ', co2, "ppm");
            display.text(CO2,0,0);
            printf(CO2); printf("\n");
            char TEMP[17];
            float temp = (gmp252.getTemperature() + hmp60.getTemperature()) / 2;
            snprintf(TEMP, 17, "%4s: %c%-5.1f %s", "Temp",
                     temp < 0 ? '-' : '+', abs(temp), "C");
            display.text(TEMP,0,9);
            printf(TEMP); printf("\n");
            char RH[17];
            float rh = hmp60.getRelativeHumidity();
            snprintf(RH, 17, "%4s:  %-5.1f %s", "RH", rh, "%");
            display.text(RH,0,18);
            printf(RH); printf("\n");

            char header[17] = "      Curr  Tar";
            display.text(header,0,36);
            printf(header); printf("\n");
            char pres[17];
            int16_t presCurr = sdp600.getPressure() / 240;
            snprintf(pres, 17, "%4s: %c%-4hd%c%c%-5.0f",
                     "Pres",
                     presCurr < 0 ? '-' : '+', abs(presCurr),
                     !manual ? '>' : ' ',
                     targetPressure < 0 ? '-' : '+', targetPressure);
            display.text(pres,0,45);
            printf(pres); printf("\n");
            char fan[17];
            float fanCurr = fanController.getFanSpeed() / 10;
            float fanTarg = targetFanSpeed / 10;
            snprintf(fan, 17, "%4s:  %-4.0f%c %-5.0f",
                     "Fan", fanCurr,
                     manual ? '>' : ' ', fanTarg);
            display.text(fan,0,54);
            printf(fan); printf("\n");
            printf("----------------------"); printf("\n");
            display.show();
            gmp252.update();
            sleep_ms(5);
            hmp60.update();
            sleep_ms(5);
        }
        while ((swEvent = picoSW.getEvent()) != NO_EVENT) {
            switch (swEvent) {
                case CLOCKWISE:
                    if (adjust) {
                        if (manual) {
                            if (targetFanSpeed < 1000) targetFanSpeed += 2;
                        } else {
                            if (targetPressure < 125) targetPressure += 1;
                        }
                    }
                    break;
                case COUNTER_CLOCKWISE:
                    if (adjust) {
                        if (manual) {
                            if (targetFanSpeed > 0) targetFanSpeed -= 2;
                        } else {
                            if (targetPressure > 0) targetPressure -= 1;
                        }
                    }
                    break;
                case ROT_PRESS:
                    adjust = !adjust;
                    break;
                case SW_0_PRESS:
                    manual = !manual;
                    break;
            }
        }
        if (manual) {
            if (fanController.getFanSpeed() != targetFanSpeed) {
                fanController.setFanSpeed(targetFanSpeed);
            }
        } else {
            int16_t currPres = sdp600.getPressure() / 240;
            if (currPres < targetPressure) {
                if (targetFanSpeed < 1000) targetFanSpeed += 2;
                cout << "Pressure adjustment!" << endl;
            } else if (currPres > targetPressure) {
                if (targetFanSpeed > 0) targetFanSpeed -= 2;
                cout << "Pressure adjustment!" << endl;
            }
        }
    }
}