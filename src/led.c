#include "led.h"
#include <zephyr/drivers/gpio.h>


void clearLEDs(const struct device *dev){
    setLED(dev,1,0);    
    setLED(dev,2,0);    
    setLED(dev,3,0); 
    setLED(dev,4,0); 
    setLED(dev,5,0); 
    setLED(dev,6,0); 
    setLED(dev,7,0); 
    setLED(dev,8,0); 
    setLED(dev,9,0); 
    setLED(dev,10,0); 
    setLED(dev,11,0); 
    setLED(dev,12,0); 
    setLED(dev,13,0); 
    setLED(dev,14,0); 
    setLED(dev,15,0); 
    setLED(dev,16,0); 
    setLED(dev,17,0); 
    setLED(dev,18,0); 
    setLED(dev,19,0); 
}

void setLED(const struct device *dev, int number, int state){

    switch(number){
        
        
        case 1:
        
            struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1),gpios);
            gpio_pin_configure_dt(&led1, GPIO_OUTPUT);
            gpio_pin_set_dt(&led1, state);
            break;
        case 2:
            struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0),gpios);
            gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
            gpio_pin_set_dt(&led0, state);
            break;
        case 3:

            struct gpio_dt_spec led16 = GPIO_DT_SPEC_GET(DT_ALIAS(led16),gpios);
            struct gpio_dt_spec led17 = GPIO_DT_SPEC_GET(DT_ALIAS(led17),gpios);
            gpio_pin_configure_dt(&led16, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led17, GPIO_OUTPUT);
            gpio_pin_set_dt(&led16, state);
            gpio_pin_set_dt(&led17, state);
            break;
        case 4:
            struct gpio_dt_spec led18 = GPIO_DT_SPEC_GET(DT_ALIAS(led18),gpios);
            struct gpio_dt_spec led19 = GPIO_DT_SPEC_GET(DT_ALIAS(led19),gpios);
            gpio_pin_configure_dt(&led18, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led19, GPIO_OUTPUT);
            gpio_pin_set_dt(&led18, state);
            gpio_pin_set_dt(&led19, state);
            break;
        case 5:
            struct gpio_dt_spec led20 = GPIO_DT_SPEC_GET(DT_ALIAS(led20),gpios);
            struct gpio_dt_spec led21 = GPIO_DT_SPEC_GET(DT_ALIAS(led21),gpios);
            gpio_pin_configure_dt(&led20, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led21, GPIO_OUTPUT);
            gpio_pin_set_dt(&led20, state);
            gpio_pin_set_dt(&led21, state);
            break;
        case 6:
            struct gpio_dt_spec led22 = GPIO_DT_SPEC_GET(DT_ALIAS(led22),gpios);
            gpio_pin_configure_dt(&led22, GPIO_OUTPUT);
            gpio_pin_set_dt(&led22, state);
            break;
        case 7:
            struct gpio_dt_spec led23 = GPIO_DT_SPEC_GET(DT_ALIAS(led23),gpios);
            gpio_pin_configure_dt(&led23, GPIO_OUTPUT);
            gpio_pin_set_dt(&led23, state);
            break;

        case 8:
            struct gpio_dt_spec led24 = GPIO_DT_SPEC_GET(DT_ALIAS(led24),gpios);
            gpio_pin_configure_dt(&led24, GPIO_OUTPUT);
            gpio_pin_set_dt(&led24, state);
            break;
        case 9:
            struct gpio_dt_spec led25 = GPIO_DT_SPEC_GET(DT_ALIAS(led25),gpios);
            gpio_pin_configure_dt(&led25, GPIO_OUTPUT);
            gpio_pin_set_dt(&led25, state);
            break;
        case 10:
            struct gpio_dt_spec led26 = GPIO_DT_SPEC_GET(DT_ALIAS(led26),gpios);
            struct gpio_dt_spec led27 = GPIO_DT_SPEC_GET(DT_ALIAS(led27),gpios);
            gpio_pin_configure_dt(&led26, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led27, GPIO_OUTPUT);
            gpio_pin_set_dt(&led26, state);   
            gpio_pin_set_dt(&led27, state);   
            break;
        case 11:
            struct gpio_dt_spec led28 = GPIO_DT_SPEC_GET(DT_ALIAS(led28),gpios);
            struct gpio_dt_spec led29 = GPIO_DT_SPEC_GET(DT_ALIAS(led29),gpios);
            gpio_pin_configure_dt(&led28, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led29, GPIO_OUTPUT);
            gpio_pin_set_dt(&led28, state);
            gpio_pin_set_dt(&led29, state);
            break;
        case 12:
            struct gpio_dt_spec led30 = GPIO_DT_SPEC_GET(DT_ALIAS(led30),gpios);
            struct gpio_dt_spec led31 = GPIO_DT_SPEC_GET(DT_ALIAS(led31),gpios);
            gpio_pin_configure_dt(&led30, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led31, GPIO_OUTPUT);
            gpio_pin_set_dt(&led30, state);
            gpio_pin_set_dt(&led31, state);
            break;
                
    
            //weekdays
        case 13:
            struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2),gpios);
            struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3),gpios);
            gpio_pin_configure_dt(&led2, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led3, GPIO_OUTPUT);
            gpio_pin_set_dt(&led2, state);
            gpio_pin_set_dt(&led3, state);
            break;
        case 14:
            struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(DT_ALIAS(led4),gpios);
            struct gpio_dt_spec led5 = GPIO_DT_SPEC_GET(DT_ALIAS(led5),gpios);
            gpio_pin_configure_dt(&led4, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led5, GPIO_OUTPUT);
            gpio_pin_set_dt(&led4, state);
            gpio_pin_set_dt(&led5, state);
            break;
        case 15:
            struct gpio_dt_spec led6 = GPIO_DT_SPEC_GET(DT_ALIAS(led6),gpios);
            struct gpio_dt_spec led7 = GPIO_DT_SPEC_GET(DT_ALIAS(led7),gpios);
            gpio_pin_configure_dt(&led6, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led7, GPIO_OUTPUT);
            gpio_pin_set_dt(&led6, state);
            gpio_pin_set_dt(&led7, state);
            break;
        case 16:
            struct gpio_dt_spec led8 = GPIO_DT_SPEC_GET(DT_ALIAS(led8),gpios);
            struct gpio_dt_spec led9 = GPIO_DT_SPEC_GET(DT_ALIAS(led9),gpios);
            gpio_pin_configure_dt(&led8, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led9, GPIO_OUTPUT);
            gpio_pin_set_dt(&led8, state);
            gpio_pin_set_dt(&led9, state);
            break;
        case 17:
            struct gpio_dt_spec led10 = GPIO_DT_SPEC_GET(DT_ALIAS(led10),gpios);
            struct gpio_dt_spec led11 = GPIO_DT_SPEC_GET(DT_ALIAS(led11),gpios);
            gpio_pin_configure_dt(&led10, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led11, GPIO_OUTPUT);
            gpio_pin_set_dt(&led10, state);
            gpio_pin_set_dt(&led11, state);
            break;
        case 18:
            struct gpio_dt_spec led12 = GPIO_DT_SPEC_GET(DT_ALIAS(led12),gpios);
            struct gpio_dt_spec led13 = GPIO_DT_SPEC_GET(DT_ALIAS(led13),gpios);
            gpio_pin_configure_dt(&led12, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led13, GPIO_OUTPUT);
            gpio_pin_set_dt(&led12, state);
            gpio_pin_set_dt(&led13, state);
            break;
        case 19:
            struct gpio_dt_spec led14 = GPIO_DT_SPEC_GET(DT_ALIAS(led14),gpios);
            struct gpio_dt_spec led15 = GPIO_DT_SPEC_GET(DT_ALIAS(led15),gpios);
            gpio_pin_configure_dt(&led14, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led15, GPIO_OUTPUT);
            gpio_pin_set_dt(&led14, state);
            gpio_pin_set_dt(&led15, state);
            break;
        
    } 
    
}
