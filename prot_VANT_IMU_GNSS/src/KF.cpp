#include <KF.h>
#include <iostream>
#include <BNO055_config.h>

// Variables para el Filtro de Kalman
double dt_IMU = 0.05; // Intervalo de tiempo (ajustar según la frecuencia de muestreo)
double dt_GPS = 0.5; // Intervalo de tiempo (ajustar según la frecuencia de muestreo)

double R3[3][3] = {{0.409581719659758, 0, 0}, {0, 0.435437894075242, 0}, {0, 0, 0.422419451721678}}; // 
double Q3[3][3] = {{0.065635956715436, 0, 0}, {0, 0.061299484337187, 0}, {0, 0, 0.054245700928569}}; // 

kfType filter;

void init_Kalman_Filter(void){
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (row == col){
                filter.P3[row][col] = 1;
                filter.x3[col] = 0;
            }
            else{
                filter.P3[row][col] = 0;
            }
        }   
    }
}

void IMU_Kalman_Filter(double A[3][3], double B[3][3], double C[3][3], 
    long double xhat_min[3][3], long double xhat[3],
    long double P_min[3][3],long double K[3][3],long double P[3][3],long double X[3]){
    /* Predicción */
    xhat_min[0][0] = (A[0][0]*X[0]) + (dt_IMU*B[0][0]*sensor3.gyrosc.x());
    xhat_min[1][1] = (A[1][1]*X[1]) + (dt_IMU*B[1][1]*sensor3.gyrosc.y());
    xhat_min[2][2] = (A[2][2]*X[2]) + (dt_IMU*B[2][2]*sensor3.gyrosc.z());

    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (row == col){
                P_min[row][col] = A[row][col]*P[row][col]*A[col][row]+Q3[row][col];  
                /* Correción */ 
                K[row][col]=(P_min[row][col]*C[col][row])/(C[row][col]*P_min[row][col]*C[col][row]+R3[row][col]);
                P[row][col] = (filter.I3[row][col]-K[row][col]*C[row][col])*P_min[row][col];
                X[row]=xhat_min[row][col]+K[row][col]*(xhat[row]-C[row][col]*xhat_min[row][col]);
            }
        }
    }
}
