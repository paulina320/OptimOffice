#ifndef _RTC_H_
#define _RTC_H_
#include <Arduino.h>
#include <ESP32Time.h>

extern ESP32Time esp_sys_time;

bool initRTC();
String getTime();
String getDate();

String getNextDay(int, int, int);
uint32_t getUnixTime();

void _set_esp_time();
void setRtcTime();

#endif