#include "SDP600.h"

SDP600::SDP600(i2c_inst *i2c, uint SDA_pin, uint SCL_pin, uint8_t devAddr) :
mSensor_i2c(i2c),
mDevAddr(devAddr)
{
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
}

int16_t SDP600::getPressure()
{
    union {
        uint16_t uint16;
        int16_t int16;
    };

    uint8_t buf[2] = {0xF1};
    i2c_write_blocking(mSensor_i2c, mDevAddr, buf, 1, true);
    i2c_read_blocking(mSensor_i2c, mDevAddr, buf, 2, false);
    uint16 = (buf[0] << 8) + (buf[1]);
    return int16;
}

void SDP600::addObserver(std::shared_ptr<Observer> observer) {
    mObservers.push_back(observer);
}

void SDP600::notifyObservers() {
    for (auto &&observer : mObservers) { observer->update(); }
}
