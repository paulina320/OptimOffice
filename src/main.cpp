#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
void temperature (void*parameters);
void sound (void*parameters);

int count1=0;
int count2=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  int result = myFunction(2, 3);

  xTaskCreate(
    temperature, //function name
    "Temperature", //task name
    1000, //stack size
    NULL, //task parameters
    1, //task priority
    NULL //task handle
  );

    xTaskCreate(
    sound, //function name
    "Sound", //task name
    1000, //stack size
    NULL, //task parameters
    1, //task priority
    NULL //task handle
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void temperature (void * parameters){
  for(;;)
    Serial.println("Temperature");
    Serial.println(count1++);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}

void sound (void * parameters){
  for(;;)
    Serial.println("Sound");
    Serial.println(count2++);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}