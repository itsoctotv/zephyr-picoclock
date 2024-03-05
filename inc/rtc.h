#ifndef RTC_H
#define RTC_H
#include <zephyr/device.h>
//rtc functions 

void updateDay(const struct device *rtc);

int setTimeAndDay(const struct device *dev, const struct device *rtc);

#endif
