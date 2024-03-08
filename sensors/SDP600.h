#ifndef PICO_MODBUS_SDP600_H
#define PICO_MODBUS_SDP600_H

#include <hardware/i2c.h>
#include <hardware/gpio.h>
#include "Subject.h"

class SDP600: public Subject {
private:
    i2c_inst * mSensor_i2c;
    uint8_t mDevAddr;
public:
    SDP600(i2c_inst *i2c = i2c1,
           uint SDA_pin = 14,
           uint SCL_pin = 15,
           uint8_t devAddr = 0x40);
    int16_t getPressure();
};


#endif //PICO_MODBUS_SDP600_H
