#include "GMP252.h"
#include "pico/time.h"

using namespace std;

GMP252::GMP252(shared_ptr<ModbusClient> modbus) :
    mCO2{ 0 },
    mTemperature{ 0 },
    mCO2RegisterLow{ modbus, mModbusAddress, CO2_REGISTER_LOW, false },
    mCO2RegisterHigh{ modbus, mModbusAddress, CO2_REGISTER_HIGH, false },
    mTemperatureRegisterLow{ modbus, mModbusAddress, TEMPERATURE_REGISTER_LOW, false },
    mTemperatureRegisterHigh{ modbus, mModbusAddress, TEMPERATURE_REGISTER_HIGH, false },
    mDeviceStatusRegister{ modbus, mModbusAddress, DEVICE_STATUS_REGISTER, false },
    mCO2StatusRegister{ modbus, mModbusAddress, CO2_STATUS_REGISTER, false }
{}

// Returns CO2 value in PPM.
float GMP252::getCO2() { return mCO2.f; }

// Returns temperature value in C.
float GMP252::getTemperature() { return mTemperature.f; }

// Returns status of the device.
uint16_t GMP252::getDeviceStatus() { return mDeviceStatusRegister.read(); }

// Returns status of the CO2 measuring.
uint16_t GMP252::getCO2Status() { return mCO2StatusRegister.read(); }

// Update sensor values.
void GMP252::update()
{
    uint32_t temp;
    mCO2.u = mCO2RegisterLow.read();
    sleep_ms(5);
    mCO2.u |= (mCO2RegisterHigh.read() << 16);
    sleep_ms(5);
    mTemperature.u = mTemperatureRegisterLow.read();
    sleep_ms(5);
    mTemperature.u |= (mTemperatureRegisterHigh.read() << 16);
    sleep_ms(5);
}