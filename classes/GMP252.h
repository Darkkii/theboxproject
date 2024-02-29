#ifndef GMP252_H
#define GMP252_H

#include <memory>
#include "ModbusClient.h"

class GMP252
{
private:
    float mCO2MeasurementPPM;
    float mTemperatureCompensationC;
    float mTemperatureMeasurementC;
    int16_t mCO2MeasurementPPMLimited;
    int16_t mCO2MeasurementPPMScaled;
    std::shared_ptr<ModbusClient> mModbus;

public:
    GMP252(/* args */);
    GMP252(const GMP252 &) = delete;
};

#endif /* GMP252_H */