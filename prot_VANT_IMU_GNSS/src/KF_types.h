#ifndef __KF_TYPES_H
#define __KF_TYPES_H
#include <iostream>
#include "vector.h"

struct kfType {
    long double x3[3] = {0, 0, 0}; // 
    long double P3[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // Matriz de covarianza del error de estimación
    long double xhat_m3[3][3]; // 
    long double P_m3[3][3]; // Predicción
    long double K3[3][3]; // Ganancia de Kalman 3x3
    double I3[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // A = I3, C = I3
    double O3[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // 
};



#endif