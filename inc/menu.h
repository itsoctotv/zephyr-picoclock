#ifndef MENU_H
#define MENU_H
#include <zephyr/device.h>
#include <stdbool.h>
//functions that change the whole screen aka switch to specified menus
int switchToMenu(const struct device *dev);
int switchToTemp(const struct device *display_device, const struct device *temp_device);
int switchToDate(const struct device *dev, const struct device *rtc);


//flags
//Flags
extern bool f_autolight;
extern bool f_twelveHourClock;

#endif
