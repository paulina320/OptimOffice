#include <ESPWiFi.h>

bool ESP_WiFi::init()
{
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(10);
    if (WiFi.isConnected())
        return true;
    else
        return false;
}

bool ESP_WiFi::check_connection()
{
    if (WiFi.isConnected()) // Check WiFi connection
        return true;
    else
    {
        log_e("WiFi not connected. Establishing connection ");
        init();
    }
    return true;
}

ESP_WiFi wf;