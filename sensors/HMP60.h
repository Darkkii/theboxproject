#ifndef HMP60_H
#define HMP60_H

#include <vector>
#include <memory>

#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Sensor32bitRegister.h"

class HMP60
{
private:
    const int mModbusAddress = 241;
    Sensor32bitRegister mRelativeHumidity;
    Sensor32bitRegister mTemperature;
    ModbusRegister mHumidityRegisterLow;
    ModbusRegister mHumidityRegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;
    ModbusRegister mErrorStatusRegister;
    ModbusRegister mErrorCodeRegisterLow;
    ModbusRegister mErrorCodeRegisterHigh;
    enum modbusRegisterAddress
    {
        RH_REGISTER_LOW = 0x0000,
        RH_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0002,
        TEMPERATURE_REGISTER_HIGH,
        ERROR_STATUS_REGISTER = 0x0200,
        ERROR_CODE_REGISTER_LOW = 0x0203,
        ERROR_CODE_REGISTER_HIGH
    };

public:
    HMP60(std::shared_ptr<ModbusClient> modbus);
    HMP60(const HMP60 &) = delete;
    float getRelativeHumidity();
    float getTemperature();
    uint32_t getErrorStatus();
    void update();
};

#endif /* HMP60_H */
