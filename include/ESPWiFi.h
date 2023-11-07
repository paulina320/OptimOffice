#ifndef __ESPWIFI_H__
#define __ESPWIFI_H__
#include <WiFi.h>
#include <Arduino.h>

#define WIFI_SSID "OptimOffice"
#define WIFI_PASS "12344321"

class ESP_WiFi
{
public:
    bool init();
    bool check_connection();
};

extern ESP_WiFi wf;

#endif