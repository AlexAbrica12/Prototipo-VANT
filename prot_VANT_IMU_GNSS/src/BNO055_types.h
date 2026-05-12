#ifndef __BNO055_TYPES_H
#define __BNO055_TYPES_H
#include <iostream>
#include "vector.h"
#include "quaternion.h"

struct imuType{
  int8_t temp;
  uint8_t calb_system;
  uint8_t calb_gyro;
  uint8_t calb_accel;
  uint8_t calb_mag;

  imu::Vector<3> acceler;
  imu::Vector<3> magnet;
  imu::Vector<3> gyrosc;
  imu::Vector<3> euler;
  imu::Vector<3> lineal_accel;
  imu::Quaternion quat;
  double quat_w;
  double quat_x;
  double quat_y;
  double quat_z;

  double xPos;
  double yPos;
  double zPos;
  double headingVel;

  int16_t accel_offset[3];
  int16_t mag_offset[3];
  int16_t gyro_offset[3];

  int16_t acceler_radiu;
  int16_t magnet_radiu;

  long double dir_acceler[3];
  long double magnitud_acceler;
  long double dir_magnet[3];
  long double magnitud_magnet;
  long double north[3];
  long double east[3];
  long double down[3]; 
  long double angles[3];
};

#endif