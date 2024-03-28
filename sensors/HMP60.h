#ifndef HMP60_H
#define HMP60_H

#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Sensor32bitRegister.h"

#include <memory>

// Handles the HMP60 humidity sensor.
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

    enum modbusRegisterAddress
    {
        RH_REGISTER_LOW = 0x0000,
        RH_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0002,
        TEMPERATURE_REGISTER_HIGH
    };

  public:
    HMP60(std::shared_ptr<ModbusClient> modbus);
    HMP60(const HMP60 &) = delete;
    float getRelativeHumidity();
    float getTemperature();
    void update();
};

#endif /* HMP60_H */
