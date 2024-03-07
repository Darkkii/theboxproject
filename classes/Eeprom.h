#ifndef EEPROM_H
#define EEPROM_H

#include <memory>
#include "I2CHandler.h"

class Eeprom
{
private:
    const int mI2CBusNumber = 0;
    const uint8_t mI2CDeviceAddress = 0x50;
    uint8_t mCurrentReadWriteAddress[2];
    std::shared_ptr<I2CHandler> mI2CHandler;
    void mSetReadWriteAddress(const uint16_t readWriteAddress);
    void mWaitUntilReady();

public:
    Eeprom(std::shared_ptr<I2CHandler> i2cHandler);
    void write(const uint8_t *writeBuffer, size_t bytesToWrite);
    void write(const uint16_t readWriteAddress, const uint8_t *writeBuffer, size_t bytesToWrite);
    void read(uint8_t *readBuffer, size_t bytesToRead);
    void read(const uint16_t readWriteAddress, uint8_t *readBuffer, size_t bytesToRead);
};

#endif /* EEPROM_H */
