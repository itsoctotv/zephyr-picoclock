#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include "display.h"
#include "io.h"


void updateAutolight(const struct device *dev, const struct adc_dt_spec adc){

	uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};


	adc_channel_setup_dt(&adc);
	

		
	int32_t val_mv; //in mili Volts
	adc_sequence_init_dt(&adc, &sequence);
	int err = adc_read_dt(&adc, &sequence);
	if (err < 0) {
		printk("Could not read (%d)\n", err);
    		
	}
	/*
	 * If using differential mode, the 16 bit value
	 * in the ADC sample buffer should be a signed 2's
	 * complement value.
	 */
	if (adc.channel_cfg.differential) {
		val_mv = (int32_t)((int16_t)buf);
	} 
	else {
		val_mv = (int32_t)buf;
	}
	
	adc_raw_to_millivolts_dt(&adc, &val_mv);
	/* conversion to mV may not be supported, skip if not */
    //printf("current mV: %d\n",val_mv);

    //if over 1500mV is reached dimm the display
    if(val_mv < 1700){ //bright
        dimmDisplay(dev, 100);
    }
    else if(val_mv > 1700){ //dark
        dimmDisplay(dev, 1);
    }
    
}
