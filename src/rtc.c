#include <zephyr/kernel.h>
#include <stdio.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <zephyr/drivers/gpio.h>
#include "rtc.h"
#include "led.h"
#include "display.h"
#include "def.h"
static const  struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);


void updateDay(const struct device *rtc){
    struct rtc_time currTime;

    //turn off all first (clears the previous day)


    setLED(dev, 13, 0); //mon
    setLED(dev, 14, 0); //tue
    setLED(dev, 15, 0); //wed
    setLED(dev, 16, 0); //thur
    setLED(dev, 17, 0); //fri
    setLED(dev, 18, 0); //sat
    setLED(dev, 19, 0); //sun
   



    rtc_get_time(rtc, &currTime);
    printf("currTime: %d\n",currTime.tm_wday);
    switch(currTime.tm_wday){
        case 1:
            printf("Sunday\n");
            setLED(dev, 19, 1);
            break;
        case 2:
            printf("Monday\n");
            setLED(dev, 13, 1);
            break;
        case 3:
            printf("Tuesday\n");
            setLED(dev, 14, 1);
            break;
        case 4:
            printf("Wednesday\n");
            setLED(dev, 15, 1);
            break;
        case 5:
            printf("Thursday\n");
            setLED(dev, 16, 1);
            break;
        case 6:
            printf("Friday\n");
            setLED(dev, 17, 1);
            break;
        case 7:
            printf("Saturday\n");
            setLED(dev, 18, 1);
            break;
        default:
            printf("Failed to read Weekdays!\n");
            
    }
}
int setTimeAndDay(const struct device *dev,const struct device *rtc){
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    

    struct rtc_time time; 

    rtc_get_time(rtc, &time); //make a copy of the current time so there aren't any unasigned values in the time struct

    
    clearDisplay(dev);
    clearLEDs(dev);


    bool hoursSet = false;
    bool minutesSet = false;
    bool daySet = false;

      
    int countingDays = 1; // start from 1 because there is no 0th day in the RTC
    int countingHours = 0;
    int countingMinutes = 0;
    
    printf("setting time...\n");
    //display still colon in the middle 
    displayChar(dev, POSCOL, POSY, ':'); 
    while(true){
        printf("hours...\n");
        k_msleep(250);
        while(!hoursSet){ //if not set
            //first set hours;
            
            if(gpio_pin_get_dt(&button) != 0){
                //count up
                if(countingHours > 22){
                    countingHours = 0;
                }
                else{
                    
                    countingHours++;
                }
                
                k_msleep(200); //delay a bit
                
            }
            else if(gpio_pin_get_dt(&button3) != 0){
                //count down
                if(countingHours < 1){
                    countingHours = 23;
                }
                else{
                    
                    countingHours--;
                }
                
                k_msleep(200); //delay a bit
                
            }
            
            if(gpio_pin_get_dt(&button2) != 0){
                //when the middle button is pressed go to next
                hoursSet = true;
                k_msleep(200);
            }
            //printf("hours current: %d\n", countingHours);
            if(countingHours > 9){
            
                uint8_t bcd = bin2bcd(countingHours);
                uint8_t mask = 0b00001111;
    
                uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30; 
                uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  
                
                displayChar(dev, POS1, POSY, firstDigit);
                displayChar(dev, POS2, POSY, secondDigit);
     
            }
            else if(countingHours < 10){
                clearChar(dev, POS1, POSY); //when counting down again
                displayChar(dev, POS2, POSY, countingHours + '0');
            }
        }
        k_msleep(250);
        printf("minutes...\n");
        while(!minutesSet){
            //then set minutes
            if(gpio_pin_get_dt(&button) != 0){
                //count up
                if(countingMinutes > 58){
                    countingMinutes = 0;
                }
                else{
                    countingMinutes++;
                    
                }
                k_msleep(200); //delay a bit
                
            }
            else if(gpio_pin_get_dt(&button3) != 0){
                //count down
                if(countingMinutes < 1){
                    countingMinutes = 59;
                }
                else{
                    countingMinutes--;
                    
                }
                k_msleep(200); //delay a bit
                
            }
            if(gpio_pin_get_dt(&button2) != 0){
                //when the middle button is pressed go to next
                minutesSet = true;
            }
            //printf("minute current: %d\n", countingMinutes);
            if(countingMinutes > 9){
            
                uint8_t bcd = bin2bcd(countingMinutes);
                uint8_t mask = 0b00001111;
    
                uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30; 
                uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  
                
                displayChar(dev, POS3, POSY, firstDigit);
                displayChar(dev, POS4, POSY, secondDigit);
     
            }
            else if(countingMinutes < 10){
                clearChar(dev, POS3, POSY); //when counting down again
                displayChar(dev, POS4, POSY, countingMinutes + '0');
            }
        }
        k_msleep(250);
        printf("days...\n");
        setLED(dev, 13, 1); //turn on monday led to signal the change to weekdays
        while(!daySet){
            /*
            *   sunday = 1
            *   monday = 2
            *   tuesday = 3
            *   wednesday = 4
            *   thursday = 5
            *   friday = 6
            *   saturday = 7 
            */
            //set the day (top led bar)
            if(gpio_pin_get_dt(&button) != 0){
                //count up
                if(countingDays > 6){
                    
                    countingDays = 1;
                    
                    
                }
                else{
                    
                    countingDays++;
                    
                    
                }
                k_msleep(200); //delay a bit
                
                
                
                //maybe come up with a better plan to this:
                setLED(dev, 13, 0); 
                setLED(dev, 14, 0);
                setLED(dev, 15, 0);
                setLED(dev, 16, 0);
                setLED(dev, 17, 0);
                setLED(dev, 18, 0);
                setLED(dev, 19, 0);
            
                switch(countingDays){
                //it just easier to do a switch statement 

                    case 1: //sunday
                        setLED(dev, 19, 1);
                        break;
                    case 2: //monday
                        setLED(dev, 13, 1);
                        break;
                    case 3: //tuesday
                        setLED(dev, 14, 1);
                        break;
                    case 4: //wednesday
                        setLED(dev, 15, 1);
                        break;          
                    case 5: //thursday
                        setLED(dev, 16, 1);
                        break;
                    case 6: //friday
                        setLED(dev, 17, 1);
                        break;
                    case 7: //saturday
                        setLED(dev, 18, 1);
                        break;
                }
            }   
            else if(gpio_pin_get_dt(&button3) != 0){
                //count down
                if(countingDays < 2){
                    countingDays = 7;
                }
                else{
                    
                    countingDays--;
                }
                
                k_msleep(200); //delay a bit

                //maybe come up with a better plan to this:
                setLED(dev, 13, 0); 
                setLED(dev, 14, 0);
                setLED(dev, 15, 0);
                setLED(dev, 16, 0);
                setLED(dev, 17, 0);
                setLED(dev, 18, 0);
                setLED(dev, 19, 0);
                switch(countingDays){
                //it just easier to do a switch statement 

                    case 1: //sunday
                        setLED(dev, 19, 1);
                        break;
                    case 2: //monday
                        setLED(dev, 13, 1);
                        break;
                    case 3: //tuesday
                        setLED(dev, 14, 1);
                        break;
                    case 4: //wednesday
                        setLED(dev, 15, 1);
                        break;          
                    case 5: //thursday
                        setLED(dev, 16, 1);
                        break;
                    case 6: //friday
                        setLED(dev, 17, 1);
                        break;
                    case 7: //saturday
                        setLED(dev, 18, 1);
                        break;
                }
                
                                
                
            }
            
            if(gpio_pin_get_dt(&button2) != 0){
                //when the middle button is pressed go to next
                daySet = true;
                k_msleep(200);
            }
            //printf("days: %d\n", countingDays);

            
        }
            


        
        if(hoursSet && minutesSet && daySet){
            break; //all set
        }
        
        
    }
    printf("hours: %d\n", countingHours);
    printf("minutes: %d\n", countingMinutes);
    printf("days: %d\n", countingDays);

    time.tm_hour = countingHours;
    time.tm_min = countingMinutes;
    time.tm_sec = 0; // set seconds to zero to reset the second counter once a new minute is set 
    time.tm_wday = countingDays;
    rtc_set_time(rtc,&time); 
    printf("time all set!\n");
    
    

    return 0; 
}
