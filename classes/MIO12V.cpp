#include "MIO12V.h"
#include "pico/time.h"

using namespace std;

MIO12V::MIO12V(shared_ptr<ModbusClient> modbus) :
    mFanSpeed{ 0 },
    mFanRotationRegister{ modbus, mModbusAddress, mFanRotationRegisterAddress, false },
    mFanSpeedRegister{ modbus, mModbusAddress,  mFanSpeedRegisterAddress }
{
    mFanSpeed = mFanSpeedRegister.read();
    sleep_ms(5);
}

uint16_t MIO12V::getFanRotation()
{
    return mFanRotationRegister.read();
}

uint16_t MIO12V::getFanSpeed()
{
    return mFanSpeed;
}

void MIO12V::setFanSpeed(int speed)
{
    if (speed < 0 || speed > 1000)
    {
        speed = speed < 1000 ? 0 : 1000;
    }

    mFanSpeed = speed;
    mFanSpeedRegister.write(mFanSpeed);
    sleep_ms(5);
}