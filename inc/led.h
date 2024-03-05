#ifndef LED_H
#define LED_H
#include <zephyr/device.h>
//functions for status leds


void setLED(const struct device *dev, int number, int state);
void clearLEDs (const struct device *dev);


#endif

