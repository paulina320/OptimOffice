#ifndef __ESP_MQTT_H__
#define __ESP_MQTT_H__

#include "UbidotsEsp32Mqtt.h"
#include <ESPMQTT.h>

const char *UBIDOTS_TOKEN = "BBUS-ZgCfmwYaEvsZdUjCuincKWJmCG2BRG";
const char *DEVICE_LABEL = "OptimOffice";

Ubidots ubidots(UBIDOTS_TOKEN);

void initCloud()
{
    // ubidots.setDebug(true);  // uncomment this to make debug messages available
    ubidots.setup();
    ubidots.connect();
}

#endif