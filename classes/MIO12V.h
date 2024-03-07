#ifndef MIO12V_H
#define MIO12V_H

#include <vector>
#include <memory>
#include "ModbusClient.h"
#include "ModbusRegister.h"
#include "Observer.h"
#include "Subject.h"

class MIO12V : public Subject
{
private:
    const int mModbusAddress = 1;
    const int mFanRotationRegisterAddress = 0x0004;
    const int mFanSpeedRegisterAddress = 0x0000;
    uint16_t mFanSpeed;
    ModbusRegister mFanRotationRegister;
    ModbusRegister mFanSpeedRegister;
    std::vector<std::shared_ptr<Observer>> mObservers;

public:
    MIO12V(std::shared_ptr<ModbusClient> modbus);
    MIO12V(const MIO12V &) = delete;
    uint16_t getFanRotation();
    uint16_t getFanSpeed();
    void setFanSpeed(int speed);
    void addObserver(std::shared_ptr<Observer> observer) override;
    void notifyObservers() override;
};

#endif /* MIO12V_H */
