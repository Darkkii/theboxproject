#include "MIO12V.h"

using namespace std;

MIO12V::MIO12V(shared_ptr<ModbusClient> modbus) :
    mFanRotationRegister{ modbus, mModbusAddress, mFanRotationRegisterAddress, false },
    mFanSpeedRegister{ modbus, mModbusAddress,  mFanSpeedRegisterAddress }
{}

uint16_t MIO12V::getFanRotation()
{
    return mFanRotationRegister.read();
}

uint16_t MIO12V::getFanSpeed()
{
    return mFanSpeedRegister.read();
}

void MIO12V::setFanSpeed(uint16_t speed)
{
    if (speed < 0 || speed > 1000)
    {
        speed = speed < 1000 ? 0 : 1000;
    }

    mFanSpeedRegister.write(speed);
}