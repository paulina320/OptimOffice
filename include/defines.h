#ifndef __DEFINES_H__
#define __DEFINES_H__
#include <Arduino.h>

enum flags_
{
    temp_humid_f,
    sound_f,
    ccs_f,
    adc_f,
    acq,
    cloud_f,
    cloud_setup_f,
    rtc_f,
    sd_f
};
extern byte flags[9];

#endif