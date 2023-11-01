#include <Temp_Humid.h>

Adafruit_Si7021 Si7021Data;

bool TempHumid::initSensor(void)
{
    if (!Si7021Data.begin())
    {
        log_e("Couldn't find a Si7021 sensor, check wiring!");
        return false;
    }
    return true;
}

bool TempHumid::getData(void)
{
    temp = Si7021Data.readTemperature();
    humid = Si7021Data.readHumidity();

    log_d("Temperature = %.2f°C\tHumidity = %.2f%", temp, humid);

    if (temp && humid)
        return true;
    return false;
}

TempHumid tempHumid;