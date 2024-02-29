#include "GMP252.h"

using namespace std;

GMP252::GMP252(shared_ptr<ModbusClient> modbus) :
    mCO2Register{ modbus, MODBUS_ADDRESS, CO2_REGISTER },
    mTemperatureRegister{ modbus, MODBUS_ADDRESS, TEMPERATURE_REGISTER },
    mDeviceStatusRegister{ modbus, MODBUS_ADDRESS, DEVICE_STATUS_REGISTER },
    mCO2StatusRegister{ modbus, MODBUS_ADDRESS, CO2_STATUS_REGISTER }
{
    // mCO2 = mCO2Register.read();
    // mTemperature = mTemperatureRegister.read();
}

// Returns CO2 value in PPM.
float GMP252::getCO2()
{
    return mCO2Register.read();;
}

// Returns temperature value in C.
float GMP252::getTemperature()
{
    return mTemperatureRegister.read();;
}

uint16_t GMP252::getDeviceStatus()
{
    return mDeviceStatusRegister.read();
}

uint16_t GMP252::getCO2Status()
{
    return mCO2StatusRegister.read();
}

// void GMP252::update()
// {
//     mCO2 = mCO2Register.read();
//     mTemperature = mTemperatureRegister.read();
// }