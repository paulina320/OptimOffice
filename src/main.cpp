#include <Arduino.h>
#include <FreeRTOS.h>
#include <Temp_Humid.h>
#include <ESPWiFi.h>
#include <rtc.h>

// #define STORE_DATA

#define DATA_ACQUISITION_TIME 1000
#define DATA_STORAGE_TIME 10

TaskHandle_t dataTask, wifiTask;
SemaphoreHandle_t semaAqData, semaWifi;

void vAcquireData(void *pvParameters);
void vWifiTransfer(void *pvParameters);

#ifdef STORE_DATA
  TaskHandle_t storageTask;
  SemaphoreHandle_t semaStorage;
  void vStorage(void *parameters);
#endif

bool rtc_flag = 0;

void setup()
{
  Serial.begin(115200);

  if (tempHumid.initSensor())
    log_d("Temp-Humidity Sensor Initialized Succesfully");
  else
  {
    log_d("Please Check Connections of Si7021");
    while (1);
  }

  if (wf.init())
    log_d("WiFi initialized succesfully");
  else
  {
    log_e("Check Network Connection");
    while (1)
    {
      if (WiFi.isConnected())
        break;
    }
  }

  if (initRTC())
  {
    rtc_flag = 1;
    _set_esp_time();
  }
  else
    rtc_flag = 0;

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

    // TODO: WHOLE LOGIC HERE
    if (tempHumid.getData())
    {
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
    // TODO: WHOLE LOGIC HERE

    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    log_v("Stack usage of WiFi Task: %d", (int)uxHighWaterMark);
    vTaskDelay(10);
  }
}

/*
void temperature(void *parameters);
void sound(void *parameters);

void temperature(void *parameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;)
    Serial.println("Temperature");
    Serial.println(count1++);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void sound(void *parameters)
{
  for (;;)
    Serial.println("Sound");
    Serial.println(count2++);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
*/