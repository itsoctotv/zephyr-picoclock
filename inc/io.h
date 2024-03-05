#ifndef IO_H
#define IO_H
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
//functions for I/O on the board (LDR, Buzzer etc...)
void updateAutolight(const struct device *dev, const struct adc_dt_spec adc);

#endif
