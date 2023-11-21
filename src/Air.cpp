#include <Arduino.h>
#include <Air.h>

Air air;

CCS811 ccs811(-1, 0x5B); // WAKE pin bound to GND
uint16_t co2_t, tvoc_t;

bool Air::initSensor(void)
{
    Wire.begin();
    ccs811.set_i2cdelay(50);
    if (ccs811.begin())
    {
        if (ccs811.start(CCS811_MODE_1SEC))
        {
            uint8_t counter = 0;
            while (!isDataValid || counter < 30)
            {
                ccs811.read(&co2_t, &tvoc_t, &errstat, &raw);
                if (errstat == CCS811_ERRSTAT_OK)
                    isDataValid = true;
                counter++;
                delay(1000);
            }
            log_d("CCS811 Initialization Successful");
            return true;
        }
    }
    return false;
}

bool Air::getData()
{
    ccs811.read(&co2_t, &tvoc_t, &errstat, &raw);
    if (errstat == CCS811_ERRSTAT_OK)
    {
        co2 = co2_t;
        tvoc = tvoc_t;
        log_d("CO2: %d ppm, TVOC: %d ppb", co2, tvoc);
        return true;
    }
    else if (errstat == CCS811_ERRSTAT_OK_NODATA)
        log_d("Waiting for CCS data");
    else if (errstat & CCS811_ERRSTAT_I2CFAIL)
        log_e("CCS811 I2C error");
    else
        log_e("CCS811 Error # %x = %s", errstat, String(ccs811.errstat_str(errstat)).c_str());
    return false;
}