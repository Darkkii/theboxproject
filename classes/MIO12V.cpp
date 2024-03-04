#include "MIO12V.h"

using namespace std;

MIO12V::MIO12V(shared_ptr<ModbusClient> modbus) :
    mFanSpeedRegister{ modbus, mModbusAddress, mFanSpeedRegisterAddress, true }
{}

void MIO12V::setFanSpeed(uint16_t speed)
{
    mFanSpeedRegister.write(speed);
}
