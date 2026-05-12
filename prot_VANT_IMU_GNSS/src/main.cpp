#define _TASK_MICRO_RES
#include <TaskScheduler.h>
#include "BNO055_config.h"
#include "SD_config.h"
#include <esp_task_wdt.h>
#include "GPS_config.h"
#include <Wire.h>
#include "TinyGPS++.h"
#include <OLED_config.h>

#define LED 2    // define appropriate pin for your board
TaskHandle_t task1_handle = NULL;
TaskHandle_t task2_handle = NULL;
TaskHandle_t task3_handle = NULL;
unsigned long previousMillis = 0; 
volatile long startMillis, endMillis, delayMillis,
              startMillis2, endMillis2, delayMillis2,
              startMillis3, endMillis3, delayMillis3;
struct Button {
    const uint8_t PIN;
    bool pressed;
};
Button button1 = {15, false};
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 
void IRAM_ATTR isr(){
button_time = millis();
 if (button_time - last_button_time > 350){
    button1.pressed = true;
    last_button_time = button_time;
  }
}

/* Leer IMU */
void task1(void * parameters){
  for(;;){
    volatile long startMillis = millis();
    Read_imu_data();
    volatile long delayMillis = millis()-startMillis;
      if (delayMillis < 50){ 
        vTaskResume(task2_handle);
        vTaskDelay((50-delayMillis) / portTICK_PERIOD_MS);
    }
  }
}

/* Escribir dato en la SD */
void task2(void * parameters){
  for(;;){
    volatile long startMillis2 = millis();
    logSDCard();      
    vTaskSuspend(task2_handle);
    volatile long delayMillis2 = millis()-startMillis2;
  }
}
/* Guardar en memoria SD */
void task3(void * parameters){
  for(;;){ 
    volatile long startMillis3 = millis();
    Read_gps();
    volatile long delayMillis3 = millis()-startMillis3;
    if (delayMillis3 < 500){
      vTaskDelay((500-delayMillis3) / portTICK_PERIOD_MS);
      }
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(LED , OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
  Serial.println("Scheduler TEST");
  while (inti_SD()==false){
    delay(1000);
  }
  digitalWrite(LED, HIGH);
  Init_OLED();
  imu_init();
  check_calib(); 
  delay(3000);
  digitalWrite(LED, LOW);
  while(button1.pressed==false){
    delay(1000);
  }
  digitalWrite(LED, HIGH);
  xTaskCreatePinnedToCore(task1, // Function to implement the task //
                          "Task1", // Name of the task //
                          10000,  // Stack size in words //
                          NULL,  // parameter of the task //
                          1,  // Priority of the task //
                          &task1_handle,  // Task handle. //
                          1); // Core where the task should run //
  xTaskCreatePinnedToCore(task2, // Function to implement the task
                          "Task2", // Name of the task 
                          10000,  // Stack size in words 
                          NULL,  // parameter of the task 
                          1,  // Priority of the task //
                          &task2_handle,  // Task handle. //
                          1); // Core where the task should run //
  vTaskSuspend(task2_handle);                   
  /*xTaskCreatePinnedToCore(task3, // Function to implement the task 
                          "Task3", // Name of the task 
                          10000,  // Stack size in words 
                          NULL,  // parameter of the task
                          1,  // Priority of the task 
                          &task3_handle,  // Task handle.
                          0); // Core where the task should run */
}

void loop(){
  delay(1000);
  print_speed();
}
