#include "SD_config.h"
#include <iostream>
#include "BNO055_config.h"
#include <GPS_config.h>
#include <ESP32Time.h>
ESP32Time rtc2(-21600);  // offset in seconds GMT-6
String dataMessage;
String log_tag;
u_int8_t addres_ID;
int fileIndex = 0;

const int maxSize = 10 * 1024 * 1024;  // Tamaño máximo del archivo en bytes (10MB)
//const int maxSize = 500 * 1024;  // Tamaño máximo del archivo en bytes (500KB)

bool inti_SD(){
  // Inicializar SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return false;    // init failed
  }
  log_tag  = gps1.day + gps1.mounth + gps1.year; //Inicializa un nuevo log por fecha

  // Busca el último archivo de log existente
  fileIndex = 0;
  while (SD.exists(getFileName(fileIndex))) {
    if(sizeFile(SD, getFileName(fileIndex))){
      fileIndex++;  //es mayor el archivo, gernera uno nuevo
      Serial.println("Archivo pesado");
    }
    else{
      //Serial.println("Reutilizando archivo");
      break;  //es menor el archivo, sal y reutilizalo
    }
  } 

  // Crea un nuevo archivo de log
  File file = SD.open(getFileName(fileIndex));
  //File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, getFileName(fileIndex), "Time,accel_x ,accel_y ,accel_z,gyro_x ,gyro_y ,gyro_z ,mag_x,mag_y,mag_z,euler_x,euler_y,euler_z,angle_x,angle_y,angle_z,angle_x,angle_y,angle_z,  \r\n");
    //writeFile(SD, "/log.txt", "");
  }
  else {

    Serial.println("File already exists");  
  }
  file.close();
  return true;
}

// Escribe el modulo de la tarjeta SD
void logSDCard() { 
  
  String tim, acc[3],mag[3],gy[3],eur[3],angle[3],quat[4],datagps[8],imu_pos[3],offset[4],kalman[3];
  //Prueba con IMU + GP
  /* Tiempo por modulo GNSS */
  //tim = gps2.hours+gps2.minutes+gps2.seconds+"."+gps2.microseconds+  ",";
  /* Tiempo por lapsosn de tiempo */
  tim = millis();
  acc[0] = String(sensor3.acceler.x(),2) + ","; acc[1] = String(sensor3.acceler.y(),2) + ","; acc[2] = String(sensor3.acceler.z(),2) + ",";
  gy[0] = String(sensor3.gyrosc.x(),4) + ","; gy[1] = String(sensor3.gyrosc.y(),4) + ","; gy[2] = String(sensor3.gyrosc.z(),4) + ",";
  mag[0] = String(sensor3.magnet.x(),4) + ","; mag[1] = String(sensor3.magnet.y(),4) + ","; mag[2] = String(sensor3.magnet.z(),4) + ",";
  eur[0] = String(sensor3.euler.x(),4) + ",";eur[1] = String(sensor3.euler.y(),4) + ","; eur[2] = String(sensor3.euler.z(),4) + ",";
  angle[0] = String((double)sensor3.angles[0],4) + ","; angle[1] = String((double)sensor3.angles[1],4) + ","; angle[2] = String((double)sensor3.angles[2],4) + ",";
  kalman[0] = String((double)filter.x3[0],4) + ","; kalman[1] = String((double)filter.x3[1],4) + ","; kalman[2] = String((double)filter.x3[2],4) + ",";
  datagps[0] = String(gps2.latitud,15) + ","; datagps[1] = String(gps2.longitud,15) + ",";
  datagps[2] =  String(gps3.pdop) + ","; datagps[3] = String(gps3.hdop) + ","; datagps[4] = String(gps3.vdop) + ",";
  datagps[5] = String(gps2.sat)+ "," + String(gps2.altitud,2) + "," ;

  //File file = SD.open("/data.txt", FILE_APPEND);
  File file = SD.open(getFileName(fileIndex), FILE_APPEND);
  file.print(tim);
  file.print(acc[0]);file.print(acc[1]);file.print(acc[2]);
  file.print(gy[0]);file.print(gy[1]);file.print(gy[2]);
  file.print(mag[0]);file.print(mag[1]);file.print(mag[2]);
  file.print(eur[0]);file.print(eur[1]);file.print(eur[2]);
  file.print(angle[0]);file.print(angle[1]);file.print(angle[2]);
  file.print(kalman[0]);file.print(kalman[1]);file.print(kalman[2]);
  file.print(datagps[0]);
  file.print(datagps[1]);
  file.print(datagps[2]);
  file.print(datagps[3]);
  file.print(datagps[4]);
  file.println(datagps[5]);
  file.close();

  // Compara tamaño del archivo actual
  if(sizeFile(SD, getFileName(fileIndex))){
  fileIndex++;  //es mayor el archivo, gernera uno nuevo
  Serial.println("Creating file...");
  writeFile(SD, getFileName(fileIndex), "Time ,Temp (DHT) ,Temp (Bar) ,Temp (IMU) ,Humidity ,Pressure ,Press_Sealevel ,Altitude ,Real_Altitude ,accel_x ,accel_y ,accel_z ,gyro_x ,gyro_y ,gyro_z ,mag_x ,mag_y ,mag_z  \r\n");
  }
  else{
    //Serial.println("Reutilizando archivo"); //es menor el archivo, sal y reutilizalo
  }
}

// Escribe a la tarjeta SD (NO MODIFICAR ESTA FUNCIÓN)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Escribe a la tarjeta SD (NO MODIFICAR ESTA FUNCIÓN)
void writeFile(fs::FS &fs, const String& path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Agregar dato a la tarjeta SD (NO MODIFICAR ESTA FUNCIÓN)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  File file = fs.open(path, FILE_APPEND);
  /*if(!file) {
    //Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    //Serial.println("Message appended");
  } else {
    //Serial.println("Append failed");
  }*/

  file.close();
}

bool sizeFile(FS &fs, const String& path)
{
  File file = fs.open(path, FILE_APPEND);
  int fileSize = file.size();
    if (fileSize >= maxSize) {
      //Serial.print("Archivo grande, ");
      //Serial.print(fileSize);Serial.print(" vs ");Serial.println(maxSize);
      file.close();
      return true;
    }
    else{
      //Serial.print("Archivo pequeño, ");
      //Serial.print(fileSize);Serial.print(" vs ");Serial.println(maxSize);
      file.close();
      return false;
    }
}

String getFileName(int index) {
  //log_tag = "/"+tiemp.date+".txt";  //Inicializa un nuevo log por fecha
  return "/" + log_tag + "_" + String(index) + ".txt";
}

String getLatestFileName() {
  return getFileName(getCurrentFileIndex());
}

int getCurrentFileIndex() {
  int index = 0;
  while (SD.exists(getFileName(index + 1))) {
    index++;
  }
  return index;
}