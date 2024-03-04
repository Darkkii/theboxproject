#ifndef HMP60_H
#define HMP60_H

#include "ModbusClient.h"
#include "ModbusRegister.h"

class HMP60
{
private:
    const int mModbusAddress = 241;
    enum modbusRegisterAddress
    {
        RH_REGISTER_LOW = 0x0100,   // 0x0000 for 32bit, 0x0100 for 16bit access.
        RH_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0002,
        TEMPERATURE_REGISTER_HIGH,
        ERROR_STATUS_REGISTER = 0x0200,
        ERROR_CODE_REGISTER_LOW = 0x0203,
        ERROR_CODE_REGISTER_HIGH
    };
    float mRelativeHumidity;
    float mTemperature;
    ModbusRegister mHumidityRegisterLow;
    ModbusRegister mHumidityRegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;
    ModbusRegister mErrorStatusRegister;
    ModbusRegister mErrorCodeRegisterLow;
    ModbusRegister mErrorCodeRegisterHigh;

public:
    HMP60(std::shared_ptr<ModbusClient> modbus);
    HMP60(const HMP60 &) = delete;
    float getRelativeHumidity();
    float getTemperature();
    uint32_t getErrorStatus();
    void update();
};

#endif /* HMP60_H */
