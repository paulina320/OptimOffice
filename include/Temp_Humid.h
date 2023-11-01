#ifndef __TEMP_HUMID__
#define __TEMP_HUMID__

#include <Wire.h>
#include <Adafruit_Si7021.h>

class TempHumid
{
    public:
        float temp;
        float humid;
        bool initSensor(void);
        bool getData(void);
};

extern TempHumid tempHumid;

#endif