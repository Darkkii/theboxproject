#include "MIO12V.h"

#include "pico/time.h"

using namespace std;

MIO12V::MIO12V(shared_ptr<ModbusClient> modbus) :
    mFanSpeed{ 0 },
    mFanRotationRegister{
        modbus, mModbusAddress, mFanRotationRegisterAddress, false
    },
    mFanSpeedRegister{ modbus, mModbusAddress, mFanSpeedRegisterAddress }
{
    mFanSpeed = mFanSpeedRegister.read();
    sleep_ms(5);
}

uint16_t MIO12V::getFanRotation() { return mFanRotationRegister.read(); }

uint16_t MIO12V::getFanSpeed() { return mFanSpeed; }

void MIO12V::setFanSpeed(int speed)
{
    if (mFanSpeed == 0)
    {
        mFanSpeedRegister.write(1000);
        sleep_ms(5);
    }
    if (speed < 0 || speed > 1000)
    {
        speed = speed < 1000 ? 0 : 1000;
    }

    mFanSpeed = speed;
    mFanSpeedRegister.write(mFanSpeed);
    sleep_ms(5);
    notifyObservers();
}

void MIO12V::addObserver(shared_ptr<Observer> observer)
{
    mObservers.push_back(observer);
}

void MIO12V::notifyObservers()
{
    for (auto &&observer : mObservers)
    {
        observer->update();
    }
}