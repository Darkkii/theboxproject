#include "Eeprom.h"

using namespace std;

Eeprom::Eeprom(std::shared_ptr<I2CHandler> i2cHandler) :
    mCurrentReadWriteAddress{ 0 },
    mI2CHandler{ i2cHandler }
{}

void Eeprom::write(const uint16_t readWriteAddress,
                   const std::string &writeBuffer)
{
    mSetReadWriteAddress(readWriteAddress);
    size_t totalBytesToWrite = writeBuffer.length() + 3; // + 2 address bytes
    uint8_t buffer[totalBytesToWrite];

    for (size_t i = 0; i < totalBytesToWrite; i++)
    {
        if (i < 2)
        {
            buffer[i] = mCurrentReadWriteAddress[i];
        }
        else
        {
            buffer[i] = writeBuffer.c_str()[i - 2];
        }
    }
    buffer[totalBytesToWrite - 1] = '\0';

    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber),
                       mI2CDeviceAddress,
                       buffer,
                       totalBytesToWrite,
                       false);
}

std::string Eeprom::read(const uint16_t readWriteAddress)
{
    char buffer[64];
    mSetReadWriteAddress(readWriteAddress);
    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber),
                       mI2CDeviceAddress,
                       mCurrentReadWriteAddress,
                       2,
                       true);
    i2c_read_blocking(mI2CHandler->getI2CBus(mI2CBusNumber),
                      mI2CDeviceAddress,
                      (uint8_t *)buffer,
                      62,
                      false);

    return std::string(buffer);
}

void Eeprom::mSetReadWriteAddress(const uint16_t readWriteAddress)
{
    mCurrentReadWriteAddress[0] = (uint8_t)(readWriteAddress >> 8);
    mCurrentReadWriteAddress[1] = (uint8_t)readWriteAddress;
}

void Eeprom::mWaitUntilReady()
{
    uint8_t temp[] = { 0 };

    while (i2c_read_timeout_us(mI2CHandler->getI2CBus(mI2CBusNumber),
                               mI2CDeviceAddress,
                               temp,
                               1,
                               false,
                               10000) == PICO_ERROR_GENERIC)
    {
        // Loop while EEPROM is unreadable or timeout is reached.
        sleep_us(500); // Sleep to stop too many repeated requests.
    }
}