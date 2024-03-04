#include "HMP60.h"

using namespace std;

HMP60::HMP60(shared_ptr<ModbusClient> modbus)
    : mHumidityRegisterLow{ modbus, mModbusAddress, RH_REGISTER_LOW },
    mHumidityRegisterHigh{ modbus, mModbusAddress, RH_REGISTER_HIGH },
    mTemperatureRegisterLow{ modbus, mModbusAddress, TEMPERATURE_REGISTER_LOW },
    mTemperatureRegisterHigh{ modbus, mModbusAddress, TEMPERATURE_REGISTER_HIGH },
    mErrorStatusRegister{ modbus, mModbusAddress, ERROR_STATUS_REGISTER },
    mErrorCodeRegisterLow{ modbus, mModbusAddress, ERROR_CODE_REGISTER_LOW },
    mErrorCodeRegisterHigh{ modbus, mModbusAddress, ERROR_CODE_REGISTER_HIGH }
{
    this->update();
}

float HMP60::getRelativeHumidity() { return mRelativeHumidity; };

float HMP60::getTemperature() { return mTemperature; }

uint32_t HMP60::getErrorStatus()
{
    uint32_t result = 0;

    if (mErrorStatusRegister.read() == 0)
    {
        result = mErrorCodeRegisterLow.read();
        result += mErrorCodeRegisterHigh.read() << 16;
    }

    return result;
}
void HMP60::update()
{
    mRelativeHumidity = mHumidityRegisterLow.read();
    mRelativeHumidity += mHumidityRegisterHigh.read() << 16;
    mTemperature = mTemperatureRegisterLow.read();
    mTemperature += mTemperatureRegisterHigh.read() << 16;
}