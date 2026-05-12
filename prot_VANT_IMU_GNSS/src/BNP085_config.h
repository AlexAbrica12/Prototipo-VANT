#ifndef BNP085_CONFIG_H
#define BNP085_CONFIG_H

#include "BNP085_types.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>

extern barometricType sensor2;

void barometric_init(void);
void Read_Barometric(float sealevelPressure);

#endif