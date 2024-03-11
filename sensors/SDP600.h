#ifndef PICO_MODBUS_SDP600_H
#define PICO_MODBUS_SDP600_H

#include <vector>

#include <hardware/i2c.h>
#include <hardware/gpio.h>


class SDP600 {
private:
    i2c_inst * mSensor_i2c;
    uint8_t mDevAddr;
    union {
        uint16_t mUint16;
        int16_t mInt16;
    } mPressure;
public:
    SDP600(i2c_inst *i2c = i2c1,
           uint SDA_pin = 14,
           uint SCL_pin = 15,
           uint8_t devAddr = 0x40);
    void update();
    int16_t getPressure() const;
};


#endif //PICO_MODBUS_SDP600_H
