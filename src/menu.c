#include <zephyr/device.h>
#include <stdbool.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/rtc.h>
#include "menu.h"
#include "led.h"
#include "display.h"
#include "def.h"


static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);
static const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
static const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(clock_rtc));


//Flags
bool f_autolight = true;
bool f_twelveHourClock = false;



int switchToTemp(const struct device *display_device, const struct device *temp_device){
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    

    clearDisplay(display_device);
    clearLEDs(display_device);

    int currentPosVal1 = POS1;
    int currentPosVal2 = POS3;

    setLED(dev, 7, 1); //turn on C led
    bool isFahrenheit = false;
    
    while(true){
        struct sensor_value temp;
        sensor_sample_fetch(temp_device);
        sensor_channel_get(temp_device, SENSOR_CHAN_AMBIENT_TEMP, &temp);

        printf("%d.%d°C\n",temp.val1,temp.val2);
        
        int tempFormatted1 = temp.val1;
        if(isFahrenheit){
            tempFormatted1 = (tempFormatted1 * 1.8)+32; //calculate to fahrenheit
            
        }
        //first value of temp
        if(tempFormatted1 < 10){
            clearChar(display_device,currentPosVal1,POSY); //clear previous char
           
            currentPosVal1 = POS2; //update location 
            clearChar(display_device,currentPosVal1,POSY);
            displayChar(display_device, currentPosVal1, POSY, tempFormatted1 + '0');
           
            currentPosVal1 = POS1;
        
        }
        
        else if(tempFormatted1 > 9){
        //if tempFormatted1 is double digit
            
            uint8_t bcd = bin2bcd(tempFormatted1);

            uint8_t mask = 0b00001111;
    
            uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30;    //mask the first 4bit
            uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  //mask the second 4bit and add value to go to the 0-9 values in the ascii table            

            currentPosVal1 = POS1;
            
            clearChar(display_device,POS1,POSY);
            displayChar(display_device, POS1, POSY, firstDigit);
            clearChar(display_device,POS2, POSY);
            displayChar(display_device, POS2, POSY, secondDigit);    
        }

        int tempFormatted2 = temp.val2;
        
        printf("tempformatted 1: %d\n", tempFormatted2);
        tempFormatted2 = tempFormatted2 / 10000;
        if(isFahrenheit){
            tempFormatted2 = (tempFormatted2 * 1.8)+32;
            if(tempFormatted2 > 99){
                tempFormatted2 = tempFormatted2 / 10;
                printf("tempppp: %d\n",tempFormatted2);
            }
            
           
        }
        printf("tempformatted 2: %d\n", tempFormatted2);
        
        //first value of temp2
        if(tempFormatted2 < 10){
            clearChar(display_device,currentPosVal2,POSY); //clear previous char
            displayChar(display_device, currentPosVal2, POSY, '0'); //set it to 0 so it doesnt look empty
                   
            currentPosVal2 = POS4; //update location 
            clearChar(display_device,currentPosVal2,POSY);
            displayChar(display_device, currentPosVal2, POSY, tempFormatted2 + '0');
                
            currentPosVal1 = POS3;
            printf("TEMPval2 %d\n",temp.val2);
              
        }
        
        else if(tempFormatted2 > 9){
            uint8_t bcd = bin2bcd(tempFormatted2);

            uint8_t mask = 0b00001111;
    
            uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30;    //mask the first 4bit
            uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  //mask the second 4bit and add value to go to the 0-9 values in the ascii table            

            currentPosVal2 = POS3;
            clearChar(display_device,POS3,POSY);
            displayChar(display_device, POS3, POSY, firstDigit);
            clearChar(display_device,POS4, POSY);
            displayChar(display_device, POS4, POSY, secondDigit);    
        }
        //draw the dot in the middle and the 'C'

        //draw dot
        clearChar(display_device, POSCOL, POSY);
        displayChar(display_device, POSCOL, POSY, '.');

        k_msleep(500);//wait a bit 
        
        int val = gpio_pin_get_dt(&button);
        if(val != 0){
            printf("exit\n");
            setLED(dev, 7, 0); //C turn off LED when exiting
            setLED(dev, 6, 0); //F
            break;
        }
        int val2 = gpio_pin_get_dt(&button2);
        if(val2 != 0){
            if(isFahrenheit){
                printf("switching back to C\n");
                setLED(dev, 6, 0);
                setLED(dev, 7, 1);
                isFahrenheit = false;
            }
            else{
                
                printf("switching to fahrenheit\n");
                setLED(dev, 7, 0);

                setLED(dev, 6, 1);
                isFahrenheit = true;
            }
        }
        int val3 = gpio_pin_get_dt(&button3);
        if(val3 != 0){
            switchToDate(dev, rtc);
            setLED(dev, 7, 10);
        }
        
    }
    clearDisplay(display_device);
    return 0;
    
    
}
int switchToDate(const struct device *dev, const struct device *rtc){
    clearDisplay(dev);
    clearLEDs(dev);
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);

    struct rtc_time time;

    rtc_get_time(rtc, &time);
    while(true){
        k_msleep(250);
        
        displayChar(dev,POS1,POSY,'0');
        displayChar(dev,POS2,POSY,time.tm_mday + '0');
        displayChar(dev,POS3,POSY,'0');
        displayChar(dev,POS4,POSY,time.tm_mon + '0');
        displayChar(dev,POSCOL,POSY,'.');
        int val = gpio_pin_get_dt(&button3);

        if(val != 0){
            break;
        }
            
                
    }
    clearDisplay(dev);
    return 0;
}

int switchToMenu(const struct device *dev){
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    
    clearDisplay(dev);

    //menu for toggling various options

    //autolight display test

    while(true){
        k_msleep(250); 
        //the options name
        displayChar(dev, POS1, POSY, 'A');
        displayChar(dev, POS2, POSY, 'L');
        displayChar(dev, POSCOL, POSY, ':');
        while(true){
            
            if(f_autolight == true){
                displayChar(dev, POS3, POSY, 'O');
                displayChar(dev, POS4, POSY, 'N');
                
            }
            else if(f_autolight == false){
                displayChar(dev, POS3, POSY, 'O');
                displayChar(dev, POS4, POSY, 'F');
            }

            int toggleOption = gpio_pin_get_dt(&button2);

            if(toggleOption != 0){
                
                f_autolight = !f_autolight; //toggle the flag
                k_msleep(250);
                clearChar(dev, POS3, POSY);
                clearChar(dev, POS4, POSY);
            }
            int nextOption = gpio_pin_get_dt(&button);
            if(nextOption != 0){
                printf("goto next opion\n");
                break;
            }
        }
        k_msleep(250);
        clearDisplay(dev);
        displayChar(dev, POS1, POSY, '1');
        displayChar(dev, POS2, POSY, '2');
        displayChar(dev, POSCOL, POSY, ':');
        while(true){

            if(f_twelveHourClock == true){
                displayChar(dev, POS3, POSY, 'O');
                displayChar(dev, POS4, POSY, 'N');
                
            }
            else if(f_twelveHourClock == false){
                displayChar(dev, POS3, POSY, 'O');
                displayChar(dev, POS4, POSY, 'F');
                
            }
            int toggleOption = gpio_pin_get_dt(&button2);
            
            if(toggleOption != 0){
                
                f_twelveHourClock = !f_twelveHourClock; //toggle the flag
                k_msleep(250);
                clearChar(dev, POS3, POSY);
                clearChar(dev, POS4, POSY);
            }
            int nextOption = gpio_pin_get_dt(&button);
            if(nextOption != 0){
                printf("goto next opion\n");
                break;
            }
        }

        printf("going back to clock\n");          
        break;
        
        
    }
    return 0;
    
        
}
