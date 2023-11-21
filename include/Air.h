#ifndef __AIR_H__
#define __AIR_H__

#include <Wire.h>
#include "ccs811.h"

class Air
{
public:
    uint16_t co2;
    uint16_t tvoc;
    bool initSensor(void);
    bool getData(void);
private:
    bool isDataValid = false;
    uint16_t errstat, raw;
};

extern Air air;

#endif