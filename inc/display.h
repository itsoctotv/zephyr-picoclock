#ifndef DISPLAY_H
#define DISPLAY_H

#include <zephyr/device.h>
// display specific functions 
int displayChar(const struct device *dev, int x, int y, char c);
int clearChar(const struct device *dev, int index, int y);
int clearDisplay(const struct device *dev);
void blinkChar(const struct device *dev, int x, int y, char c);
int dimmDisplay(const struct device *dev, uint8_t brightness);


#endif
