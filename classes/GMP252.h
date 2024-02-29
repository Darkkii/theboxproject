#ifndef GMP252_H
#define GMP252_H

// #include "ModbusClient.h"

class GMP252
{
private:
    float mCO2Measurement;
    float mTemperatureCompensation;
    float mTemperatureMeasurement;
    // uint

public:
    GMP252(/* args */);
    GMP252(const GMP252 &) = delete;
};

#endif /* GMP252_H */