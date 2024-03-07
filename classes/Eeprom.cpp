#include "Eeprom.h"

using namespace std;

Eeprom::Eeprom(std::shared_ptr<I2CHandler> i2cHandler) :
    mCurrentReadWriteAddress{ 0 },
    mI2CHandler{ i2cHandler }
{}

void Eeprom::write(const uint8_t *writeBuffer, size_t bytesToWrite)
{
    size_t totalBytesToWrite = bytesToWrite + 2; // +2 address bytes
    uint8_t buffer[totalBytesToWrite];

    for (int i = 0; i < totalBytesToWrite; i++)
    {
        if (i < 2)
        {
            buffer[i] = mCurrentReadWriteAddress[i];
        }
        else
        {
            buffer[i] = writeBuffer[i - 2];
        }
    }

    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, buffer, totalBytesToWrite, false);
}

void Eeprom::write(const uint16_t readWriteAddress, const uint8_t *writeBuffer, size_t bytesToWrite)
{
    mSetReadWriteAddress(readWriteAddress);
    write(writeBuffer, bytesToWrite);
}

void Eeprom::read(uint8_t *readBuffer, size_t bytesToRead)
{
    mWaitUntilReady();
    i2c_write_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, mCurrentReadWriteAddress, 2, true);
    i2c_read_blocking(mI2CHandler->getI2CBus(mI2CBusNumber), mI2CDeviceAddress, readBuffer, bytesToRead, false);
}

void Eeprom::read(const uint16_t readWriteAddress, uint8_t *readBuffer, size_t bytesToRead)
{
    mSetReadWriteAddress(readWriteAddress);
    read(readBuffer, bytesToRead);
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

/*

// Reads data from the EEPROM.
int eeprom_read(uint8_t *dest, size_t bytes_to_read)
{
    eeprom_wait_for_ready();
    i2c_write_blocking(eeprom_i2c, EEPROM_DEVICE_ADDRESS, rwaddr, 2, true);

    return i2c_read_blocking(eeprom_i2c, EEPROM_DEVICE_ADDRESS, dest, bytes_to_read, false);
}

void eeprom_clear_bytes(size_t bytes_to_clear)
{
    uint8_t clear_buffer[EEPROM_PAGE_MAX] = {0};
    eeprom_write(clear_buffer, bytes_to_clear);
    eeprom_wait_for_ready();
}

*/