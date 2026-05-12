#ifndef BNO055_CONFIG_H
#define BNO055_CONFIG_H

#include "BNO055_types.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>
#include "KF.h"

/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (40)

/* Set the option for calibration */
#define INIT_CALIB        false
#define RUNNING_CALIB     true

#define PRINT_DELAY_MS  500U
#define DEG_2_RAD       0.01745329251 //trig functions require radians, BNO055 outputs degrees
#define ACCEL_VEL_TRANSITION  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0
#define ACCEL_POS_TRANSITION  0.5*ACCEL_VEL_TRANSITION*ACCEL_VEL_TRANSITION

extern imuType sensor3;

void imu_init(void);
void check_calib(void);
void init_calib(void);
void running_calib(void);
void Read_imu_data(void);
void Read_imu2(void);
void Read_Quat(void);
void Read_offset(void);

void printEvent(sensors_event_t* event);
void Read_imu_temp(void);

#endif