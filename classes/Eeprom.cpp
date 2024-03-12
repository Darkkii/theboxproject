#include "Eeprom.h"

using namespace std;

Eeprom::Eeprom(std::shared_ptr<I2CHandler> i2cHandler) :
    mCurrentReadWriteAddress{ 0 },
    mI2CHandler{ i2cHandler }
{}

void Eeprom::write(const uint16_t readWriteAddress, const std::string &writeBuffer, size_t bytesToWrite)
{
    mSetReadWriteAddress(readWriteAddress);
    size_t totalBytesToWrite = bytesToWrite + 2; // +2 address bytes
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

    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, buffer, totalBytesToWrite, false);
}

std::string Eeprom::read(const uint16_t readWriteAddress, size_t bytesToRead)
{
    char buffer[64];
    mSetReadWriteAddress(readWriteAddress);
    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, mCurrentReadWriteAddress, 2, true);
    i2c_read_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, (uint8_t *)buffer, bytesToRead, false);

    return buffer;
}

void Eeprom::mSetReadWriteAddress(const uint16_t readWriteAddress)
{
    mCurrentReadWriteAddress[0] = (uint8_t)(readWriteAddress >> 8);
    mCurrentReadWriteAddress[1] = (uint8_t)readWriteAddress;
}

void Eeprom::mWaitUntilReady()
{
    uint8_t temp[] = { 0 };

    while (i2c_read_timeout_us(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, temp, 1, false, 10000) == PICO_ERROR_GENERIC)
    {
        // Loop while EEPROM is unreadable or timeout is reached.
        sleep_us(500); // Sleep to stop too many repeated requests.
    }
}