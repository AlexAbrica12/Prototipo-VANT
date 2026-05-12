#include <OLED_config.h>
#include <GPS_config.h>
#include <ESP32Time.h>
#include <BNO055_config.h>

ESP32Time rtc3(-21600);  // offset in seconds GMT-6
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Init_OLED()
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.display();
    delay(2000);
}

void print_speed()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
    
  display.setCursor(30, 5);
  display.print(gps2.hours);

  display.setCursor(45, 5);
  display.print(gps2.minutes);

  display.setCursor(60, 5);
  display.print(gps2.seconds);

  display.setCursor(75, 5);
  display.print(".");
  display.setCursor(90, 5);
  display.print(gps2.microseconds);


  display.setCursor(10, 20);
  display.print("Lat: ");
  display.setCursor(25, 20);
  display.print(gps2.latitud,6);

  display.setCursor(10, 30);
  display.print("Lng: ");
  display.setCursor(25, 30);
  display.print(gps2.longitud,6);

  display.setCursor(0, 40);
  display.print("Angl: ");
  display.setCursor(25, 40);
  display.print(sensor3.euler.x(),2);

  display.setCursor(55, 40);
  display.print("Sat: ");
  display.setCursor(80, 40);
  display.print(gps2.sat);

  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("HDOP:");
  display.setCursor(25, 50);
  display.print(gps3.hdop);

  display.setTextSize(1);
  display.setCursor(60, 50);
  display.print("ALT:");
  display.setCursor(85, 50);
  display.print(gps2.altitud);
  display.display();

}

void print_imu(uint8_t gyro, uint8_t acce, uint8_t mag, uint8_t syst)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(30, 5);
  display.print("CALIBRATION IMU:");

  display.setCursor(25, 20);
  display.print("Gyro: ");
  display.setCursor(55, 20);
  display.print(gyro, DEC);

  display.setCursor(25, 30);
  display.print("Acel: ");
  display.setCursor(55, 30);
  display.print(acce, DEC);

  display.setCursor(25, 40);
  display.print("Magn: ");
  display.setCursor(55, 40);
  display.print(mag, DEC);

  display.setCursor(25, 50);
  display.print("Syst:");
  display.setCursor(55, 50);
  display.print(syst, DEC);

  display.display();

}