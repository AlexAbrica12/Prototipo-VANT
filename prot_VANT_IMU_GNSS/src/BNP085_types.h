#ifndef BNP085_TYPES_H
#define BNP085_TYPES_H

#include <iostream>

/*  Definir configuración del Barometro  */
struct barometricType {
  float Temp;
  int32_t Pressure;
  float Altitude;
  float Real_Altitude;
  int32_t Press_Sealevel;
};

#endif