#ifndef KF_H
#define KF_H

#include <iostream>
#include "KF_types.h"
#include "vector.h"
#include "math.h"

extern kfType filter;

void init_Kalman_Filter(void);
void IMU_Kalman_Filter(double A[3][3],double B[3][3],double C[3][3], 
    long double xhat_min[3][3],long double xhat[3],
    long double P_min[3][3],long double K[3][3],long double P[3][3],long double X[3]);
void GPS_Kalman_Filter(void);

#endif