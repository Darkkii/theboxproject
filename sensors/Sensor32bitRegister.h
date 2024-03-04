#ifndef SENSOR32BITREGISTER_H
#define SENSOR32BITREGISTER_H

#include <cstdint>

union Sensor32bitRegister
{
    uint32_t u;
    float f;
};

#endif /* SENSOR32BITREGISTER_H */