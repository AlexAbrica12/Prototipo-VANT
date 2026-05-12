#include "BNO055_config.h"
#include <OLED_config.h>
#include <utility/imumaths.h>

// Dirección I2C del dispositivo
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28, &Wire);
imuType sensor3;
adafruit_bno055_offsets_t calibrationData;
adafruit_bno055_offsets_t newCalib;

void imu_init(){
    if(!bno.begin()){
        Serial.print("Ooops, no se detectada el BNO055...");
        while(1);
    }
    delay(1000);
    //bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_P7);
    //bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P7);
    bno.setExtCrystalUse(true);
    Serial.println("Valores de estatus de calibración: 0=descalibrado, 3=calibrado");
}

void check_calib(void){
  int eeAddress = 0;
  long bnoID;
  bool foundCalib = false;

  EEPROM.get(eeAddress, bnoID);
  sensor_t sensor;
  /*
  *  Look for the sensor's unique ID at the beginning oF EEPROM.
  */
  bno.getSensor(&sensor);
  if (bnoID != sensor.sensor_id)
  {
      Serial.println("\nDatos de calibración no encontrados en EEPROM");
      delay(500);
  }
  else
  {
    Serial.println("\nDatos de calibración encontrados en EEPROM");
    eeAddress += sizeof(long);
    EEPROM.get(eeAddress, calibrationData);
    Serial.println("\n\nRestaurando datos de calibración al EEPROM..."); 
    bno.setSensorOffsets(calibrationData);
    Serial.println("\n\nDatos de calibración cargados a la EEPROM");
    foundCalib = true;
  }  
  delay(1000);

  sensors_event_t event;
  bno.getEvent(&event);  
  if (foundCalib){
    Serial.println("Mueva el sensor ligeramente para calibrar el magnetómetro");
    while (!bno.isFullyCalibrated())
    {
      bno.getEvent(&event);
      delay(BNO055_SAMPLERATE_DELAY_MS);
    }
  }
  else
  {
    Serial.println("Iniciar calibración de la IMU: ");
    init_calib();
  }

  Serial.println("\nCalibración Completado");
  Serial.println("--------------------------------");
  Serial.println("Resultado de la calibración: ");
  bno.getSensorOffsets(newCalib);

  Serial.println("\n\nGuadar datos de calibración al EEPROM...");

  eeAddress = 0;
  bno.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(eeAddress, bnoID);

  eeAddress += sizeof(long);
  EEPROM.put(eeAddress, newCalib);
  Serial.println("Guadar datos");

  Serial.println("\n--------------------------------\n");
  delay(500);
}

void init_calib(){
  bool calb_flag = false;
  Serial.println("Mueva el sensor ligeramente para calibrar sus sensores");
  /* Espera hasta que se calibren todos los modulos/sensores del IMU */
  while(!calb_flag)
  {
    running_calib();
    if((sensor3.calb_system == 3) && (sensor3.calb_gyro==3) && (sensor3.calb_accel==3) && (sensor3.calb_mag==3))
      calb_flag = true;
    else   
      calb_flag = false; 
  }
  init_Kalman_Filter();
}

void running_calib(){
  bno.getCalibration(&sensor3.calb_system, &sensor3.calb_gyro, &sensor3.calb_accel, &sensor3.calb_mag);
  Serial.print("CALIBRATION: Sys=");
  Serial.print(sensor3.calb_system, DEC);
  Serial.print(" Gyro=");
  Serial.print(sensor3.calb_gyro, DEC);
  Serial.print(" Accel=");
  Serial.print(sensor3.calb_accel, DEC);
  Serial.print(" Mag=");
  Serial.println(sensor3.calb_mag, DEC);
  print_imu(sensor3.calb_gyro, sensor3.calb_accel, sensor3.calb_mag, sensor3.calb_system);
  delay(BNO055_SAMPLERATE_DELAY_MS);
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);
}

void Read_imu_temp(){
  sensor3.temp = bno.getTemp();
}

void Read_imu_data(){
  /* Vectores de los sensores de la IMU */
  sensor3.acceler = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER); //Accel (m/s^2)
  sensor3.gyrosc = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);      //Gyro (rad/s)
  sensor3.magnet = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);   //Magnet (uT) 
  sensor3.euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);           //Euler (°)

  /* Calculo de la magnitud del acelerometro */
  sensor3.magnitud_acceler = sqrtl((sensor3.acceler.x()*sensor3.acceler.x()) +
                                   (sensor3.acceler.y()*sensor3.acceler.y()) +
                                   (sensor3.acceler.z()*sensor3.acceler.z()));
  if(sensor3.magnitud_acceler == 0)
    sensor3.magnitud_acceler = 0.00001;
  sensor3.magnitud_magnet = sqrtl((sensor3.magnet.x())*(sensor3.magnet.x()) +
                                  (sensor3.magnet.y())*(sensor3.magnet.y()) + 
                                  (sensor3.magnet.z())*(sensor3.magnet.z()));
  if(sensor3.magnitud_magnet == 0)
    sensor3.magnitud_magnet = 0.00001;
  /* Calculo de los vectores de Down */
  sensor3.dir_acceler[0] = sensor3.acceler.x()/sensor3.magnitud_acceler;  //sensor3.down[0]
  sensor3.dir_acceler[1] = sensor3.acceler.y()/sensor3.magnitud_acceler;  //sensor3.down[1]
  sensor3.dir_acceler[2] = sensor3.acceler.z()/sensor3.magnitud_acceler;  //sensor3.down[2]
  /* Calculo de los vectores de North y East */
  sensor3.dir_magnet[0] = sensor3.magnet.x()/sensor3.magnitud_magnet;
  sensor3.dir_magnet[1] = sensor3.magnet.y()/sensor3.magnitud_magnet;
  sensor3.dir_magnet[2] = sensor3.magnet.z()/sensor3.magnitud_magnet;   
  sensor3.east[0] = (sensor3.dir_acceler[1]*sensor3.dir_magnet[2])-(sensor3.dir_acceler[2]*sensor3.dir_magnet[1]);
  sensor3.east[1] = (sensor3.dir_acceler[2]*sensor3.dir_magnet[0])-(sensor3.dir_acceler[0]*sensor3.dir_magnet[2]);
  sensor3.east[2] = (sensor3.dir_acceler[0]*sensor3.dir_magnet[1])-(sensor3.dir_acceler[1]*sensor3.dir_magnet[0]); 
  sensor3.north[0] = (sensor3.east[1]*sensor3.dir_acceler[2])-(sensor3.east[2]*sensor3.dir_acceler[1]);
  sensor3.north[1] = (sensor3.east[2]*sensor3.dir_acceler[0])-(sensor3.east[0]*sensor3.dir_acceler[2]);
  sensor3.north[2] = (sensor3.east[0]*sensor3.dir_acceler[1])-(sensor3.east[1]*sensor3.dir_acceler[0]);
  /* Calculo de los ángulos de Euler */
  if((sensor3.dir_acceler[0]>-0.99)&&(sensor3.dir_acceler[0]<0.99)){
    sensor3.angles[1] = asinl(sensor3.dir_acceler[0]) * RAD_TO_DEG;
    sensor3.angles[0] = atan2l((-1)*sensor3.dir_acceler[1],sensor3.dir_acceler[2]) * RAD_TO_DEG;
    sensor3.angles[2] = (atan2l((-1)*sensor3.east[0],sensor3.north[0]) - (PI/2))* RAD_TO_DEG;
  }
  else{
    sensor3.angles[0] = 0;
    if(sensor3.dir_acceler[0] <= -0.99){
      sensor3.angles[1] = (-PI/2) * RAD_TO_DEG;
      sensor3.angles[2] = (((atan2l(sensor3.north[1],sensor3.north[2])) - (PI/2))*RAD_TO_DEG);
    }
    else if(sensor3.dir_acceler[0] >= 0.99){
      sensor3.angles[1] = (PI/2) * RAD_TO_DEG;
      sensor3.angles[2] = (((atan2l(sensor3.north[1],(-1)*sensor3.north[2])) - (PI/2))*RAD_TO_DEG); 
    }
  }
  if (sensor3.angles[2] > 360)
    sensor3.angles[2] = sensor3.angles[2] - 360;
  else if(sensor3.angles[2] < 0)
    sensor3.angles[2] = sensor3.angles[2] + 360;
  /* Aplicar filtro de Kalman a los ángulos de Euler */
  IMU_Kalman_Filter(filter.I3, filter.I3, filter.I3, filter.xhat_m3, sensor3.angles, filter.P_m3, filter.K3,filter.P3,filter.x3);
}
