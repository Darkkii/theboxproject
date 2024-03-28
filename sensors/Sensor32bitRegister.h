#ifndef SENSOR32BITREGISTER_H
#define SENSOR32BITREGISTER_H

#include <cstdint>

// Used for type punning the sensor data from integer to float.
union Sensor32bitRegister
{
    uint32_t u;
    float f;
};

#endif /* SENSOR32BITREGISTER_H */