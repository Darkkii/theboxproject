#include "Eeprom.h"

Eeprom::Eeprom(std::shared_ptr<I2CHandler> i2cHandler) :
    mCurrentReadWriteAddress{ 0 },
    mI2CHandler{ i2cHandler }
{}

void Eeprom::write(const uint8_t *writeBuffer, size_t bytesToWrite)
{

}

void Eeprom::write(const uint16_t readWriteAddress, const uint8_t *writeBuffer, size_t bytesToWrite)
{}

void Eeprom::read(uint8_t *readBuffer, size_t bytesToRead)
{}

void Eeprom::read(const uint16_t readWriteAddress, uint8_t *readBuffer, size_t bytesToRead)
{}

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

// Writes data to the EEPROM.
int eeprom_write(const uint8_t *source, size_t bytes_to_write)
{
    size_t rwaddr_offset = 2; // 2 address bytes
    size_t total_length = bytes_to_write + rwaddr_offset;
    uint8_t buffer[total_length];

    for (int i = 0; i < total_length; i++)
    {
        if (i < rwaddr_offset)
        {
            buffer[i] = rwaddr[i];
        }
        else
        {
            buffer[i] = source[i - rwaddr_offset];
        }
    }

    eeprom_wait_for_ready();
    return i2c_write_blocking(eeprom_i2c, EEPROM_DEVICE_ADDRESS, buffer, total_length, false);
}

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

// Waits until EEPROM is ready for reading/writing.
static int eeprom_wait_for_ready(void)
{
    uint8_t temp = 0;
    int result = 0;

    while ((result = i2c_read_timeout_us(eeprom_i2c, EEPROM_DEVICE_ADDRESS, &temp, 1, false, 10000)) == PICO_ERROR_GENERIC)
    {
        // Loop while EEPROM is unreadable or timeout is reached.
        sleep_us(500); // Sleep to stop too many repeated requests.
    }
    return result;
}

*/