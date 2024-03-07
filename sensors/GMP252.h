#ifndef GMP252_H
#define GMP252_H

#include <vector>
#include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Sensor32bitRegister.h"
#include "Observer.h"
#include "Subject.h"

class GMP252 : public Subject
{
private:
    const int mModbusAddress = 240;
    Sensor32bitRegister mCO2;
    Sensor32bitRegister mTemperature;
    ModbusRegister mCO2RegisterLow;
    ModbusRegister mCO2RegisterHigh;
    ModbusRegister mTemperatureRegisterLow;
    ModbusRegister mTemperatureRegisterHigh;
    ModbusRegister mDeviceStatusRegister;
    ModbusRegister mCO2StatusRegister;
    std::vector<std::shared_ptr<Observer>> mObservers;
    enum modbusRegisterAddress
    {
        CO2_REGISTER_LOW = 0x0000,
        CO2_REGISTER_HIGH,
        TEMPERATURE_REGISTER_LOW = 0x0004,
        TEMPERATURE_REGISTER_HIGH,
        DEVICE_STATUS_REGISTER = 0x0800,
        CO2_STATUS_REGISTER
    };

public:
    GMP252(std::shared_ptr<ModbusClient> modbus);
    GMP252(const GMP252 &) = delete;
    float getCO2();
    float getTemperature();
    uint16_t getDeviceStatus();
    uint16_t getCO2Status();
    void update();
    void addObserver(std::shared_ptr<Observer> observer) override;
    void notifyObservers() override;
};

#endif /* GMP252_H */
