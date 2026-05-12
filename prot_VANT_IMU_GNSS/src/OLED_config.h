#ifndef _OLED_CONFIH_H_
#define _OLED_CONFIH_H_

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//On ESP32: GPIO-21(SDA), GPIO-22(SCL)
#define OLED_RESET -1 //Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //See datasheet for Address

void Init_OLED();
void print_speed();
void print_imu(uint8_t gyro, uint8_t acce, uint8_t mag, uint8_t syst);

#endif /* _OLED_CONFIH_H_ */