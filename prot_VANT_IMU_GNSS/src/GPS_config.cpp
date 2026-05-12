#include <GPS_config.h>
#include <OLED_config.h>
#include <ESP32Time.h>
#include <Wire.h>
#include <TinyGPS++.h>

HardwareSerial gpshw(1);
ESP32Time rtc_gps(-21600);  // offset in seconds GMT-6

bool ggaFound = false;      // Bandera de mensaje GGA
bool rmcFound = false;      // Bandera de mensaje RMC
bool gsaFound = false;      // Bandera de mensaje GSA
bool gsvFound = false;      // Bandera de mensaje GSV
bool ChecksumTerm = false;  // CheckSum de mensajes del modulo Neo8mGPS
bool messagetype = false;   // Bandera para indicar si se encontró un mensaje GGA
uint8_t parity, messcount = 0;
char incomingbyte;
String gpsData, allmesage, parstring, datarmc[12], datagga[15], datagsa[18], datagsv[20];
rmcType gps1;
ggaType gps2;
gsaType gps3;
gsvType gps4;

void gps_init(void){
    bool gpsCalibrated = false;
    volatile uint8_t i = 0;
    /* Comunicación serial del Neo8mGPS */
    gpshw.begin(9600, SERIAL_8N1, RXD2, TXD2);
    GNSS_config(0);
    delay(100);
  for (unsigned long start = millis(); millis() - start < 1000;){
    while (!gpsCalibrated) {
      while (gpshw.available()){
        incomingbyte=gpshw.read();

        /* Si el caracter recibido indica inicio del mensaje */
        if (incomingbyte == '$') {
          gpsData = "";
          rmcFound = false;
          allmesage = "";
          parstring = "";
          parity=0;
          ChecksumTerm = false;
        }
        /* Si el caracter recibido indica si es un nuevo atributo en el mensaje */
        else if (incomingbyte == ',' && (rmcFound)){
          if(rmcFound==true)
            datarmc[i++] = gpsData;
          parity ^= (uint8_t)incomingbyte;
          gpsData = "";
        }
        /* Si el caracter recibido indica fin del mensaje e inicio el valor del CheckSum */
        else if (incomingbyte == '*'){
          if(rmcFound==true)
            datarmc[i] = gpsData;
          ChecksumTerm = true;
          gpsData = "";
        }
        /* Si el caracter recibido indica el final de la linea del mensaje */
        else if((incomingbyte == '\r')&& (ChecksumTerm)){
          parstring = String(parity, HEX);
          parstring.toUpperCase();
          /* Si el valor de paridad del bloque del mensaje es igual al CheckSum prupuesto */
          if(gpsData == parstring){
            if(rmcFound){
              conver_values();
              messcount++;
              rtc_gps.setTime(gps1.seconds.toInt(),gps1.minutes.toInt(),gps1.hours.toInt(),
                              gps1.day.toInt(),gps1.mounth.toInt(),gps1.year.toInt(),gps1.microseconds.toInt());
              gpsCalibrated = true;
            }
          }
          i=0;
          /* Limpiar la variable y comenzar a capturar el nuevo mensaje */
          gpsData = "";
        }
        /* Si recibimos es un caracter cualquiera */
        else{
          /* Añadir el byte recibido a la variable gpsData */
          gpsData += incomingbyte;
          if(!ChecksumTerm)
            parity ^= (uint8_t)incomingbyte;
        }
        /* Clasifique el tipo de mensaje GNSS */
        if((gpsData == "GNRMC,")||(gpsData == "GPRMC,")||(gpsData == "GLRMC,")||(gpsData == "GARMC,")){
          rmcFound = true;
          messagetype = true;
          gpsData = "";
        }
      }
    }
  }
}

void gps_setup(void)
{
  // Enable $PUBX,40,GLL,0,1,0,0*5D
  //gpshw.println("$PUBX,40,GLL,0,1,0,0*5D");
   // disable $PUBX,40,GLL,0,0,0,0*5C
  gpshw.println("$PUBX,40,GLL,0,0,0,0*5C");
  delay(100);

  // Enable $PUBX,40,RMC,0,1,0,0*46
  gpshw.println("$PUBX,40,RMC,0,1,0,0*46");  
    // disable $PUBX,40,RMC,0,0,0,0*47
 // gpshw.println("$PUBX,40,RMC,0,1,0,0*46"); on
  delay(100);

  // Enable $PUBX,40,VTG,0,1,0,0*5F
  //gpshw.println("$PUBX,40,VTG,0,1,0,0*5F");
  // disable $PUBX,40,VTG,0,0,0,0*5E
  gpshw.println("$PUBX,40,VTG,0,0,0,0*5E"); 
  delay(100);

  // Enable $PUBX,40,GGA,0,1,0,0*5B
  gpshw.println("$PUBX,40,GGA,0,1,0,0*5B");
  // disable $PUBX,40,GGA,0,1,0,0*5A
  //gpshw.println("$PUBX,40,GGA,0,0,0,0*5A"); //ojo
  delay(100);

  // Enable $PUBX,40,GSA,0,1,0,0*4F
  gpshw.println("$PUBX,40,GSA,0,1,0,0*4F");
  // disable $PUBX,40,GSA,0,0,0,0*4E
  //gpshw.println("$PUBX,40,GSA,0,0,0,0*4E");
  delay(100);

  // Enable $PUBX,40,GSV,0,1,0,0*58
  gpshw.println("$PUBX,40,GSV,0,1,0,0*58");
  // disable $PUBX,40,GSV,0,0,0,0*59
  //gpshw.println("$PUBX,40,GSV,0,0,0,0*59");
  delay(100);

  SetUp2hZ();
}

void Read_gps(void){
  volatile uint8_t i = 0;
  volatile uint16_t parint, parmssg;
  bool gsablock = false, gsvblock = false;
  messcount = 0;
  while(messcount < 4)
  {
    if (gpshw.available())
    {
      incomingbyte=gpshw.read();
      /* Si el caracter recibido indica inicio del mensaje */
      if (incomingbyte == '$') {
        gpsData = "";  // Limpiar la variable y comenzar a capturar el nuevo mensaje
        ggaFound = rmcFound = gsaFound = false; // Reiniciar la bandera de mensaje encontrado
        gsvFound = false;
        allmesage = "";
        parstring = "";
        parity=0;
        ChecksumTerm = false;
      }
      /* Si el caracter recibido indica si es un nuevo atributo en el mensaje
         y si no se repite el mismo tipo de mensaje GNSS */
      else if (incomingbyte == ',' && (ggaFound || rmcFound || gsaFound || gsvFound)){
        if(ggaFound==true)
          datagga[i++] = gpsData;
        else if(rmcFound==true)
          datarmc[i++] = gpsData;
        else if(gsaFound==true)
          datagsa[i++] = gpsData;
        else if(gsvFound==true)
          datagsv[i++] = gpsData;
        parity ^= (uint8_t)incomingbyte;
        gpsData = "";  // Limpiar la variable y comenzar a capturar el nuevo dato
      }
      /* Si el caracter recibido indica fin del mensaje e inicio el valor del CheckSum */
      else if (incomingbyte == '*'){
        if(ggaFound==true)
          datagga[i] = gpsData;
        else if(rmcFound==true)
          datarmc[i] = gpsData;
        else if(gsaFound==true)
          datagsa[i] = gpsData;
        else if(gsvFound==true)
          datagsv[i] = gpsData;
        ChecksumTerm = true;
        gpsData = "";  // Limpiar la variable y comenzar a capturar el nuevo dato
      }
      /* Si el caracter recibido indica el final de la linea del mensaje */
      else if((incomingbyte == '\r')&& (ChecksumTerm))
      {
        parstring = String(parity, HEX);
        parmssg = gpsData.toInt();
        parint = parstring.toInt();
        if(parmssg == parint){
          if(rmcFound){
            conver_values();
            messcount++;
          }
          else if(ggaFound){
            conver_values();
            messcount++;
          }
          else if(gsaFound){
            conver_values();
            messcount++;
            gsablock = true;
          }
          else if(gsvFound){
            conver_values();
            messcount++;
            gsvblock = true;
          }
        }
        i=0;
        gpsData = "";  // Limpiar la variable y comenzar a capturar el nuevo mensaje
      }
      /* Si recibimos es un caracter cualquiera */
      else{
        gpsData += incomingbyte;  // Añadir el byte recibido a la variable gpsData
        if(!ChecksumTerm)
        parity ^= (uint8_t)incomingbyte;
      }
      if((gpsData == "GNGGA,")||(gpsData == "GPGGA,")||(gpsData == "GLGGA,")||(gpsData == "GAGGA,")){
        ggaFound = true;
        messagetype = true;
        gpsData = "";
      }
      if((gpsData == "GNRMC,")||(gpsData == "GPRMC,")||(gpsData == "GLRMC,")||(gpsData == "GARMC,")){
        rmcFound = true;
        messagetype = true;
        gpsData = "";
      }                                         
      if(((gpsData == "GPGSA,")||(gpsData == "GNGSA,")||(gpsData == "$GLGSA,")||(gpsData == "GAGSA,")) && (!gsablock)){ //&& (!gsablock)
        gsaFound = true;
        messagetype = true;
        gpsData = "";
      }
      if(((gpsData == "GPGSV,")||(gpsData == "GNGSV,")||(gpsData == "$GLGSV,")||(gpsData == "GAGSV,")) && (!gsvblock)){
        gsvFound = true;
        messagetype = true;
        gpsData = "";
      }
    }
  }
}

void conver_values()
{
  volatile int8_t hours;
  bool day_correction = false, timeStamp_check = false;
  if(rmcFound)
  {
    /* Ajustar a la zona horaria de México (UTC-6) */
    hours = (datarmc[0].toInt()/10000) - 6;
    /* Asegurarse de que la hora esté en el rango correcto (0-23) */
    if (hours < 0) {
      hours += 24;
      day_correction = true; 
      //gps1.day--;
    }

    if((hours) < 10)
      gps1.hours = "0" + String(hours);
    else
      gps1.hours = String(hours);  

    if(((datarmc[0].toInt()/100)%100) < 10)
      gps1.minutes = "0" + String((datarmc[0].toInt()/100)%100);
    else
      gps1.minutes = String((datarmc[0].toInt()/100)%100);  

    if((datarmc[0].toInt()%100) < 10)
      gps1.seconds = "0" + String(datarmc[0].toInt()%100);
    else
      gps1.seconds = String(datarmc[0].toInt()%100);
    gps1.microseconds = String(datarmc[0].substring(7,9).toInt());

    if (day_correction){
      gps1.day = String((datarmc[8].toInt()/10000)-1);
      day_correction=false;
    }
    else
      gps1.day = String((datarmc[8].toInt()/10000));
    if((gps1.day.toInt()) < 10)
      gps1.day = "0" + String((datarmc[8].toInt()/10000));
    else
      gps1.day = String(gps1.day.toInt());
    if(((datarmc[8].toInt()/100)%100) < 10)
      gps1.mounth = "0" + String((datarmc[8].toInt()/100)%100);
    else
      gps1.mounth = String((datarmc[8].toInt()/100)%100); 

    if(((datarmc[8].toInt()%100) + 2000) < 10)
      gps1.year = "0" + String((datarmc[8].toInt()%100) + 2000);
    else
      gps1.year = String((datarmc[8].toInt()%100) + 2000);

    if(!timeStamp_check)
    {
      //gps1.utctime = String(rtc.getTime("%H:%M:%S")+":"+rtc.getMillis()/10);
      gps1.utcdate = String(rtc_gps.getDate());
    }
    
    gps1.utctime = datarmc[0];
    gps1.utctime2 = datarmc[0].toFloat();
    
    if (datarmc[1] == "A")
    {
      /* Conversion de los grados,minutos y segundos (sistema sexagesimal) */
      gps1.latitud = (float)datarmc[2].substring(0,2).toInt() + 
                    (datarmc[2].substring(2,4).toFloat() +
                    (datarmc[2].substring(5,10).toFloat())/100000)/60;
      if (datarmc[3] == "S")
        gps1.latitud = -gps1.latitud;

      gps1.longitud = (float)datarmc[4].substring(0,3).toInt() + 
                    (datarmc[4].substring(3,5).toFloat() +
                    (datarmc[4].substring(6,11).toFloat())/100000)/60;
      if (datarmc[5] == "W")
        gps1.longitud = -gps1.longitud; 
      /* Obtner velocidad (en Nudos) y cursor sobre "suelo" (en grados) */
      gps1.speed = (float)datarmc[6].toInt();
      gps1.course = (float)datarmc[7].toInt();
    }

    else if(datarmc[1] == "V")
    {
      //gps1.latitud = 0;
      //gps1.longitud = 0;
    }
  }

  if(ggaFound)
  {
    /* Ajustar a la zona horaria de México (UTC-6) */
    hours = (datagga[0].toInt()/10000) - 6;
    /* Asegurarse de que la hora esté en el rango correcto (0-23) */
    if (hours < 0) {
      hours += 24;
      //gps1.day--;
    }

    if((hours) < 10)
      gps2.hours = "0" + String(hours);
    else
      gps2.hours = String(hours);  

    if(((datagga[0].toInt()/100)%100) < 10)
      gps2.minutes = "0" + String((datagga[0].toInt()/100)%100);
    else
      gps2.minutes = String((datagga[0].toInt()/100)%100);  

    if((datagga[0].toInt()%100) < 10)
      gps2.seconds = "0" + String(datagga[0].toInt()%100);
    else
      gps2.seconds = String(datagga[0].toInt()%100);
    gps2.microseconds = String(datagga[0].substring(7,9).toInt());

    if ((datagga[5] == "1") || (datagga[5] == "2"))
    {
      /* Conversion de los grados,minutos y segundos (sistema sexagesimal) */
      gps2.latitud = (float)datagga[1].substring(0,2).toInt() + 
                    (datagga[1].substring(2,4).toDouble() +
                    (datagga[1].substring(5,10).toDouble())/100000)/60;
      if (datagga[2] == "S")
        gps2.latitud = -gps2.latitud;

      gps2.longitud = (float)datagga[3].substring(0,3).toInt() + 
                      (datagga[3].substring(3,5).toDouble() +
                      (datagga[3].substring(6,11).toDouble())/100000)/60;
      if (datagga[4] == "W")
        gps2.longitud = -gps2.longitud;   
    }
    else
    {
      //gps2.latitud = 0;
      //gps2.longitud = 0;    
    }
    gps2.sat = datagga[6].toInt();
    gps2.hpod = datagga[7].toFloat();
    gps2.altitud = datagga[8].toFloat();
  }

  if(gsaFound)
  {
    gps3.pdop = datagsa[14].toFloat();
    gps3.hdop = datagsa[15].toFloat();
    gps3.vdop = datagsa[16].toFloat();
  }

  if(gsvFound)
  {
    gps4.sv = datagsa[2].toInt();
  }
}

/* Configuracion de velocidad de muestreo del módulo GNSS */
/* Muestro de 1Hz*/
void SetUp1hZ()
{
  byte packet[] = {
    0xB5, // 
    0x62, // 
    0x06, // 
    0x08, // 
    0x06, // length
    0x00, // 
    0xE8, // measRate, hex 0X03E8 = dec 1000 ms
    0x03, // 
    0x01, // navRate, always =1
    0x00, // 
    0x01, // timeRef, stick to GPS time (=1)
    0x00, // 
    0x01, // CK_A
    0x39, // CK_B
  };
 for (byte i = 0; i < 14; i++){
  gpshw.write(packet[i]); // GPS is HardwareSerial
 }
}
/* Muestro de 2Hz*/
void SetUp2hZ()
{
  byte packet[] = {
    0xB5, // 
    0x62, // 
    0x06, // 
    0x08, // 
    0x06, // length
    0x00, // 
    0xF4, // measRate, hex 0X01F4 = dec 500 ms
    0x01, // 
    0x01, // navRate, always =1
    0x00, // 
    0x01, // timeRef, stick to GPS time (=1)
    0x00, // 
    0x0B, // CK_A
    0x77, // CK_B
  };
 for (byte i = 0; i < 14; i++){
  gpshw.write(packet[i]); // GPS is HardwareSerial
 }
}
/* Muestro de 4Hz*/
void SetUp4hZ()
{
  byte packet[] = {
    0xB5, // 
    0x62, // 
    0x06, // 
    0x08, // 
    0x06, // length
    0x00, // 
    0xFA, // measRate, hex 0X00FA = dec 250 ms
    0x00, // 
    0x01, // navRate, always =1
    0x00, // 
    0x01, // timeRef, stick to GPS time (=1)
    0x00, // 
    0x10, // CK_A
    0x96, // CK_B
  };
 for (byte i = 0; i < 14; i++){
  gpshw.write(packet[i]); // GPS is HardwareSerial
 }
}

/* Configuración constelaciones en el módulo GNSS */
void GNSS_config(uint8_t constelaciones)
{
    /* GPS + GLONASS + SBAS + QZSS */
    if(constelaciones==0){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01, //GLONASS +
            0x2F, 0x89,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }
    }
    /* GPS */
    else if(constelaciones==1){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, //GLONASS +
            0x2C, 0x4D,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    }
    /* GPS + SBAS + QZSS */
    else if(constelaciones==2){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, //GLONASS +
            0x2E, 0x85,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    }
    /* GLONASS */
    else if(constelaciones==3){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01, //GLONASS +
            0x2C, 0x1D,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    } 
    /* GLONASS + SBAS + QZSS */
    else if(constelaciones==4){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01, //GLONASS + 56+18=4
            0x2E, 0x55,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    }

    /* GPS + GLONASS */
    else if(constelaciones==5){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01, //GLONASS +
            0x2D, 0x51,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    } 

    /* GPS + GLONASS + SBAS + QZSS + GALILEO */
    else if(constelaciones==6){
        byte packet[] = {
            0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 
            0x00, 0x00, 0x20, 0x07,
            0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, //GPS +
            0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //SBAS +
            0x02, 0x04, 0x08, 0x00, 0x01, 0x00, 0x01, 0x01, //GALILEO 
            0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, //BEIDOU 
            0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, //IMES
            0x05, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01, //QZSS +
            0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01, //GLONASS +
            0x30, 0xAD,
        };
        for (byte i = 0; i < 68; i++){
          gpshw.write(packet[i]); // GPS is HardwareSerial
          }        
    } 
}    
