#ifndef GPS_CONFIG_H
#define GPS_CONFIG_H

#include "GPS_types.h"
#include <utility/imumaths.h>
#include "Arduino.h"

#define RXD2 16
#define TXD2 17

extern gsaType gps3;
extern ggaType gps2;
extern rmcType gps1;
extern gsvType gps4;

void gps_init(void);
void gps_setup(void);
void Read_gps(void);


void conver_values();
void SetUp1hZ();
void SetUp2hZ();
void SetUp4hZ();
void GNSS_config(uint8_t constelaciones);

#endif