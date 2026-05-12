#ifndef __GPS_TYPES_H
#define __GPS_TYPES_H
#include <iostream>
#include "vector.h"
#include "WString.h"

struct gsaType{
    float pdop;
    float hdop;
    float vdop;
};

struct ggaType{
    String hours;
    String minutes;
    String seconds;
    String microseconds;

    double latitud;

    double longitud;


    uint8_t sat;
    float hpod;
    float altitud;
};

struct rmcType{
    String utctime;
    float utctime2;
    String utcdate;
    String hours;
    String minutes;
    String seconds;
    String microseconds;

    float latitud;
    float longitud;
    float speed;
    float course;

    String day;
    String mounth;
    String year;
};

struct gsvType{
    uint8_t sv;
    String start_tim;
    String end_tim;
};

#endif