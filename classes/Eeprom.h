#ifndef EEPROM_H
#define EEPROM_H

#include <memory>
#include <string>
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
    void write(const uint16_t readWriteAddress, const std::string &writeBuffer, size_t bytesToWrite);
    std::string read(const uint16_t readWriteAddress, size_t bytesToRead);
};

#endif /* EEPROM_H */
