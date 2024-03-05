#include "HMP60.h"
#include "pico/time.h"

using namespace std;

HMP60::HMP60(shared_ptr<ModbusClient> modbus) :
    mRelativeHumidity{ 0 },
    mTemperature{ 0 },
    mHumidityRegisterLow{ modbus, mModbusAddress, RH_REGISTER_LOW, false },
    mHumidityRegisterHigh{ modbus, mModbusAddress, RH_REGISTER_HIGH, false },
    mTemperatureRegisterLow{ modbus, mModbusAddress, TEMPERATURE_REGISTER_LOW, false },
    mTemperatureRegisterHigh{ modbus, mModbusAddress, TEMPERATURE_REGISTER_HIGH, false },
    mErrorStatusRegister{ modbus, mModbusAddress, ERROR_STATUS_REGISTER, false },
    mErrorCodeRegisterLow{ modbus, mModbusAddress, ERROR_CODE_REGISTER_LOW, false },
    mErrorCodeRegisterHigh{ modbus, mModbusAddress, ERROR_CODE_REGISTER_HIGH, false }
{}

float HMP60::getRelativeHumidity() { return mRelativeHumidity.f; };

float HMP60::getTemperature() { return mTemperature.f; }

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
    mRelativeHumidity.u = mHumidityRegisterLow.read() | (mHumidityRegisterHigh.read() << 16);
    sleep_ms(5);
    mTemperature.u = mTemperatureRegisterLow.read() | (mTemperatureRegisterHigh.read() << 16);
    sleep_ms(5);
}