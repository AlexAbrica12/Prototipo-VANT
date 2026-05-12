#include "BNP085_config.h"

Adafruit_BMP085 bmp;
barometricType sensor2;

void barometric_init(void)
{
    if (!bmp.begin()) {
	    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
	    while (1) {}
  }
  delay(100);
}

void Read_Barometric(float sealevelPressure)
{
  sensor2.Temp = bmp.readTemperature(); // Temperature °C
  sensor2.Pressure = bmp.readPressure();    //  Pressure Pa
  sensor2.Altitude = bmp.readAltitude();    //  Altitude meters
  sensor2.Press_Sealevel = bmp.readSealevelPressure();  //  Pressure at sealevel (calculated) Pa
  sensor2.Real_Altitude = bmp.readAltitude(sealevelPressure);   //  Real altitude meters
}