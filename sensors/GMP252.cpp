#include "GMP252.h"

using namespace std;

GMP252::GMP252(shared_ptr<ModbusClient> modbus) :
    mCO2RegisterLow{ modbus, MODBUS_ADDRESS, CO2_REGISTER_LOW },
    mCO2RegisterHigh{ modbus, MODBUS_ADDRESS, CO2_REGISTER_HIGH },
    mTemperatureRegisterLow{ modbus, MODBUS_ADDRESS, TEMPERATURE_REGISTER_LOW },
    mTemperatureRegisterHigh{ modbus, MODBUS_ADDRESS, TEMPERATURE_REGISTER_HIGH },
    mDeviceStatusRegister{ modbus, MODBUS_ADDRESS, DEVICE_STATUS_REGISTER },
    mCO2StatusRegister{ modbus, MODBUS_ADDRESS, CO2_STATUS_REGISTER }
{
    this->update();
}

// Returns CO2 value in PPM.
float GMP252::getCO2()
{
    return mCO2;
}

// Returns temperature value in C.
float GMP252::getTemperature()
{
    return mTemperature;
}

// Returns status of the device.
uint16_t GMP252::getDeviceStatus()
{
    return mDeviceStatusRegister.read();
}

// Returns status of the CO2 measuring.
uint16_t GMP252::getCO2Status()
{
    return mCO2StatusRegister.read();
}

void GMP252::update()
{
    mCO2 = mCO2RegisterLow.read();
    mCO2 = mCO2RegisterHigh.read() << 16;
    mTemperature = mTemperatureRegisterLow.read();
    mTemperature = mTemperatureRegisterHigh.read() << 16;
}