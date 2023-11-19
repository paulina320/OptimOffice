#include <Arduino.h>
#include <FreeRTOS.h>
#include <ESPWiFi.h>
#include <ESPMQTT.h>
#include <Temp_Humid.h>
#include <noise.h>
#include <Air.h>
#include <rtc.h>
#include "ESP32Ping.h"
#include "defines.h"

const IPAddress remote_ip(8, 8, 8, 8);

// #define STORE_DATA

#define DATA_ACQUISITION_TIME 30*1000
// #define DATA_STORAGE_TIME 10

TaskHandle_t dataTask, wifiTask;
SemaphoreHandle_t semaAqData, semaWifi;

void vAcquireData(void *pvParameters);
void vWifiTransfer(void *pvParameters);

#ifdef STORE_DATA
  TaskHandle_t storageTask;
  SemaphoreHandle_t semaStorage;
  void vStorage(void *parameters);
#endif

byte flags[8];
unsigned long lastTimeStamp = 0;
float temp_t, humid_t, noise_t, CO2_t, TVOC_t = 0;
int dataparam = 0;

void setup()
{
  Serial.begin(115200);

  //* Si7021 Initialization
  { // TODO: Remove these comments for actual sensor
    // if (tempHumid.initSensor())
    //   log_d("Temp-Humidity Sensor Initialized Succesfully");
    // else
    // {
    //   log_e("Please Check Connections of Si7021");
    //   while (1);
    // }
  }

  //* Wi-Fi Initialization and Connection
  {
    if (wf.init())
      log_d("WiFi initialized succesfully");
    else
    {
      log_e("Check Network Connection");
      while (1)
      {
        if (WiFi.isConnected())
        {
          initCloud();
          log_d("Cloud IoT Setup Complete");
          flags[cloud_setup_f] = 1;
          if (ubidots.connected())
            flags[cloud_f] = 1;
          break;
        }
      }
    }
    delay(100);
  }

  //* RTC Initialization
  {
    if (initRTC())
    {
      flags[rtc_f] = 1;
      _set_esp_time();
    }
    else
      flags[rtc_f] = 0;
  }

  semaAqData = xSemaphoreCreateBinary();
  semaWifi = xSemaphoreCreateBinary();

  xSemaphoreGive(semaAqData);
  xSemaphoreGive(semaWifi);

  xTaskCreatePinnedToCore(vAcquireData, "Data Acquisition", 6000, NULL, 2, &dataTask, 1);
  xTaskCreatePinnedToCore(vWifiTransfer, "Transfer data on Wifi", 7000, NULL, 1, &wifiTask, 0);

#ifdef STORE_DATA
  semaStorage = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(vStorage, "Storage Handler", 6000, NULL, 1, &storageTask, 1);
#endif
}

void loop()
{
  vTaskDelay(10);
}

void vAcquireData(void *parameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    xSemaphoreTake(semaAqData, portMAX_DELAY);

    if (dataparam == 0)
    {
      if (1)
      {
        tempHumid.temp = 23;
        tempHumid.humid = 54;
        dataparam++;
      }

      // if (tempHumid.getData()) // TODO: Remove this comment for actual sensor
      {
        air.tvoc = float(random(200,220))/1000;
        air.co2 = random(420,440);
        dataparam++;
      }

      noise.readAudio();
      if (noise.interpretNoise())
        dataparam++;

      lastTimeStamp = getUnixTime();
      log_d("Data captured at %s", getTime().c_str());
    }

    xSemaphoreGive(semaAqData);

    #ifdef STORE_DATA
      xSemaphoreGive(semaStorage);
    #endif

    vTaskDelay(1);

    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    log_v("Stack usage of Acquisition Task: %d", (int)uxHighWaterMark);
    vTaskDelayUntil(&xLastWakeTime, DATA_ACQUISITION_TIME);
  }
}

void vWifiTransfer(void *parameters)
{
  for (;;)
  {
    if (dataparam > 1)
    {
      log_d("Entering WiFi Task");
      xSemaphoreTake(semaWifi, portMAX_DELAY);
      xSemaphoreTake(semaAqData, portMAX_DELAY);
      {
        if (tempHumid.temp != 0)
          temp_t = tempHumid.temp;
        if (tempHumid.humid != 0)
          humid_t = tempHumid.humid;
        if (noise.lastNoiseValue != 0)
          noise_t = noise.lastNoiseValue;
        if (air.co2 != 0)
          CO2_t = air.co2;
        if (air.tvoc != 0)
          TVOC_t = air.tvoc;
        dataparam = 0;
        log_d("Fetched Data for Transmission");
      }
      xSemaphoreGive(semaAqData);
      
      if (wf.check_connection())
      {
        // if (Ping.ping(remote_ip,1))
        {
          if (!flags[cloud_f] && !flags[cloud_setup_f])
          {
            initCloud();
            log_d("Cloud IoT Setup Complete");
            flags[cloud_setup_f] = 1;
          }

          ubidots.loop();

          if (!ubidots.connected())
          {
            flags[cloud_f] = 0;
            ubidots.reconnect();
          }
          if (ubidots.connected())
          {
            // if (!noise_t && !temp_t && !humid_t && CO2_t)
            {
              flags[cloud_f] = 1;

              ubidots.add("TemperatureLevel", temp_t);
              ubidots.add("Humidity", humid_t);
              ubidots.add("NoiseLevel", noise_t);
              ubidots.add("CO2", CO2_t);
              ubidots.add("TVOC", TVOC_t);

              ubidots.publish(DEVICE_LABEL);
              log_d("Sent data to cloud");
              log_d("DATA SENT: Temp=%.2fC, Humid=%.2f%%, Noise=%.2fdB, CO2=%.0fppm, TVOC=%.2fppm\n", temp_t, humid_t, noise_t, CO2_t, TVOC_t);

              temp_t = 0;
              humid_t = 0;
              noise_t = 0;
              CO2_t = 0;

              setRtcTime();
            }
          }
        }
        // else
        // {
        //   log_d("Ping Not Received");
        //   flags[cloud_f] = 0;
        // }
        xSemaphoreGive(semaWifi);
      }
      else
      {
        log_i("Wifi disconnected!");
        flags[cloud_f] = 0;
        xSemaphoreGive(semaWifi);
        vTaskDelay(10000);
      }
    }

    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    log_v("Stack usage of WiFi Task: %d", (int)uxHighWaterMark);
    vTaskDelay(1000);
  }
}
