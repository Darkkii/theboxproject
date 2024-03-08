#include <stdio.h>
#include <string.h>
#include <cmath>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "uart/PicoUart.h"

// #include "IPStack.h"
// #include "Countdown.h"
// #include "MQTTClient.h"
#include "MQTTHandler.h"
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "ssd1306.h"
#include "GMP252.h"
#include "HMP60.h"
#include "MIO12V.h"

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

#define USE_MQTT
// #define USE_MODBUS
// #define USE_SSD1306
// #define TEST_SENSORS
// #define TEST_FAN_MOTOR


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
#endif

int main()
{
#ifdef TEST_SENSORS
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };
    auto modbus_poll = make_timeout_time_ms(3000);
    GMP252 gmp252{ rtu_client };
    HMP60 hmp60{ rtu_client };
    gmp252.update();
    sleep_ms(5);
    gmp252.update();
    sleep_ms(5);
    hmp60.update();
    sleep_ms(5);
#endif

    const uint led_pin = 22;
    const uint button = 9;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");

#ifdef USE_SSD1306
    // I2C is "open drain",
    // pull ups to keep signal high when no data is being sent
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); // the display has external pull-ups
    gpio_set_function(15, GPIO_FUNC_I2C); // the display has external pull-ups
    ssd1306 display(i2c1);
    display.fill(0);
    display.text("Hello", 0, 0);
    mono_vlsb rb(raspberry26x32, 26, 32);
    display.blit(rb, 20, 20);
    display.rect(15, 15, 35, 45, 1);
    display.line(60, 5, 120, 60, 1);
    display.line(60, 60, 120, 5, 1);
    display.show();
#if 1
    for (int i = 0; i < 128; ++i) {
        sleep_ms(50);
        display.scroll(1, 0);
        display.show();
    }
    display.text("Done", 20, 20);
    display.show();
#endif

#endif

#ifdef USE_MQTT
    MQTTHandler mqttHandler;
    mqttHandler.connect();
    auto mqttTimeout = make_timeout_time_ms(2000);
#endif

#ifdef USE_MODBUS
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };
    ModbusRegister rh(rtu_client, 241, 256);
    auto modbus_poll = make_timeout_time_ms(3000);
#endif

#ifdef TEST_FAN_MOTOR
    auto uart{ std::make_shared<PicoUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE) };
    auto rtu_client{ std::make_shared<ModbusClient>(uart) };

    MIO12V fanController(rtu_client);
    sleep_ms(100);
    fanController.setFanSpeed(0);
    printf("%u\n", fanController.getFanSpeed());
#endif

    while (true) {
#ifdef USE_MODBUS
        if (time_reached(modbus_poll)) {
            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            modbus_poll = delayed_by_ms(modbus_poll, 3000);
            printf("RH=%5.1f%%\n", rh.read() / 10.0);
        }
#endif
#ifdef USE_MQTT
        if (time_reached(mqttTimeout))
        {
            // mqttHandler.update();
            mqttTimeout = make_timeout_time_ms(2000);
        }

        mqttHandler.keepAlive();
#endif

#ifdef TEST_SENSORS
        if (time_reached(modbus_poll)) {
            gpio_put(led_pin, !gpio_get(led_pin)); // toggle  led
            modbus_poll = delayed_by_ms(modbus_poll, 3000);
            printf("CO2: %.0f\n", gmp252.getCO2());
            printf("Temp (GMP252): %.1f\n", gmp252.getTemperature());
            printf("RH: %.1f\n", hmp60.getRelativeHumidity());
            printf("Temp (HMP60): %.1f\n", hmp60.getTemperature());
            printf("----------------\n");
            gmp252.update();
            sleep_ms(5);
            hmp60.update();
            sleep_ms(5);
        }
#endif
    }
}

