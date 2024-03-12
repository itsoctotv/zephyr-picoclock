#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/adc.h>

#include "def.h"
#include "display.h"
#include "temp.h"
#include "led.h"
#include "menu.h"
#include "rtc.h"
#include "temp.h"
#include "io.h"

static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
static struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);



struct display_capabilities caps;



//basically copied example from zephyr/samples/drivers/adc
//with the rpi_pico.overlay thing because nothing else worked
//in boards/rpi_pico.overlay

const struct adc_dt_spec adc_channel0 = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(clock_rtc));

const struct device *devtemp = DEVICE_DT_GET(DT_NODELABEL(clock_dts));


//Flags
//from menu.h global bool variables, menu.c sets those variables; main.c and menu.c uses them

int main() {


    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    
    struct rtc_time time;/*= {
        .tm_sec = 0,            //sec
        .tm_min = 47,           //minute
        .tm_hour = 9,          //hour
        .tm_mday = 9,          //day of month
        .tm_mon = 1,            //month
        .tm_year = 1990,        //year
        .tm_wday = 2,           //weekday
        .tm_yday = 44,          //yearday
        .tm_isdst = -1,         //daylight saving flag
        .tm_nsec = 1            //nanosec
    };
  
    
    rtc_set_time(rtc,&time);*/
    rtc_get_time(rtc, &time);
    printf("year rtc from main.c: %d\n", time.tm_year);


    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01 );
    
   
    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);

    clearDisplay(dev);

    //writing stuff
    
    displayChar(dev, POS1, POSY,'0');
    displayChar(dev, POS2, POSY,'1');
    displayChar(dev, POSCOL, POSY,':');
    displayChar(dev, POS3, POSY,2);
    displayChar(dev, POS4, POSY,3);    
    
    int hour = 0;
    int minute = 0;
    int day = 0; 
    int currentPosHr = POS1;
    int currentPosMin = POS3;
    
    clearDisplay(dev);
    
    int prevHr = -1;
    int prevMin = -1;
    int prevDay = -1;
    int returnSwitchTemp = -1;
    int returnSwitchMenu = -1;
    int returnTimeAndDay = -1;
    uint8_t val = 12;
    uint8_t bcd = bin2bcd(val);

/*
    bcd:
    dec to bin: 
    12
    0000 1100
    bcd:
dec: 1    2
    0001 0010

*/

    uint8_t mask = 0b00001111;  
    uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30;    //mask the first ,4bit
    uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  //mask the second 4bit and add value to go to the 0-9 values in the ascii table
    

    printf("first digit ascii num: %x char: %c\n", firstDigit,(char)firstDigit);
    printf("second digit ascii num: %x char: %c\n", secondDigit,(char)secondDigit);

    //turn on by default
    //bool toggleAutolight = true; 
    
    while(true){
        //printf("H: %d  M: %d  S: %d  \n",time.tm_hour, time.tm_min, time.tm_sec);
        
        //because of blinkChar() you'll have to wait until this if condition is checked in the while loop
        //just a simple example/test for the button; possible fix with "async interrupt handlers (?)"
        //https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
        //for now press the button longer a bit to trigger the if statements in main() and switchToTemp() 

        //buttons
        int val = gpio_pin_get_dt(&button);
        int val2 = gpio_pin_get_dt(&button2);
        int val3 = gpio_pin_get_dt(&button3);
        //printf("buttonval: %d\n", val);
        if(val != 0){
            
            returnSwitchTemp = switchToTemp(dev,devtemp); //switch to the temperature view and stay there until button is pressed again
            
        }
        if(val2 != 0){
            //scrollText(dev,':');


            //set time and date
            
            returnTimeAndDay = setTimeAndDay(dev,rtc);

        }

        
        if(val3 != 0){
            /*if(toggleAutolight == true){
                toggleAutolight = false;
                setLED(dev, 10, 0);
            }
            else{
                toggleAutolight = true;
                setLED(dev, 10, 1);
                
            }*/
            returnSwitchMenu = switchToMenu(dev);
            

        }
        

        if(f_autolight){
            setLED(dev, 12, 1); //if you change to temp display or clockset display the led goes of because clearLEDs() is called in those functions this turns on the hole time while toggleAutolight is true

            updateAutolight(dev, adc_channel0);
            
        }
        else{
            setLED(dev, 12, 0);
        }
        
        
        if(returnSwitchTemp == 0){
            //if returned from switchToTemp() update/set hours and minutes
            prevHr = -1;
            prevMin = -1; //set to default to simulate "restart"
            prevDay = -1;
            returnSwitchTemp = -1; //set it to -1 or any value to not trigger this statement again
            printf("display reset!\n");
        }
        if(returnSwitchMenu == 0){
            prevHr = -1;
            prevMin = -1;
            prevDay = -1;
            returnSwitchMenu = -1;
            printf("display reset!\n");

        }
        if(returnTimeAndDay == 0){
            prevHr = -1;
            prevMin = -1;
            prevDay = -1;
            returnTimeAndDay = -1;
            printf("display reset!\n");

        }

        
        rtc_get_time(rtc, &time);
        if(f_twelveHourClock == true){
            //activate am/pm func (just turn on/off leds for )
            setLED(dev, 9, 1);
            //maybe
            // https://stackoverflow.com/questions/63976824/how-to-convert-hour-number-from-24h-to-12h-format-without-any-conditional-operat
            if(time.tm_hour < 12){ //from 0-12 in 24h format
                //in the AMs
                setLED(dev, 8, 1); //turn on AM led
                setLED(dev, 9, 0); //turn off PM led

                int hourFormatted = (time.tm_hour % 12); // limit to 12
                if(hourFormatted == 0){
                    hourFormatted = 12; //take care of 0s in 24h format, they dont exist in 12h format
                }
                //printf("hour in 12h format: %d\nnormal hour: %d\n", hourFormatted, time.tm_hour);
                hour = hourFormatted;       
                

                
            }
            if(time.tm_hour > 11){ //from 12-24 in 24h format 
                //in the PMs
                
                setLED(dev, 8, 0); //turn off AM led
                setLED(dev, 9, 1); //turn on PM led
                int hourFormatted = (time.tm_hour % 12); // limit to 12
                if(hourFormatted == 0){ //take care of 0s in 24h format, they dont exist in 12h format
                    hourFormatted = 12;
                }
                //printf("hour in 12h format: %d\nnormal hour: %d\n", hourFormatted, time.tm_hour);
                hour = hourFormatted; //set the main hour variable to the formatted 12h version
                
            }
            
        }
        else if(f_twelveHourClock == false){
            
            setLED(dev, 8, 0);
            setLED(dev, 9, 0);
            hour = time.tm_hour; //dont set main hour variable to the formatted 12h version, keep it as is
        }
        
        minute = time.tm_min; 
        day = time.tm_wday;
        
            

        
        if(hour < 10 && (hour != prevHr)){
            clearChar(dev,currentPosHr,POSY); //clear previous char
            displayChar(dev, currentPosHr, POSY, '0'); //set it to 0 so it doesnt look empty
            currentPosHr = POS2; //update location 
            clearChar(dev,currentPosHr,POSY);
            displayChar(dev, currentPosHr, POSY, hour + '0'); //converting it to a char so it looks up the ascii table 
            prevHr = hour;
            currentPosHr = POS1;

        }
//seperate 2digit int into 2 seperate ints via bcd 
        else if(hour > 9 && (hour != prevHr)){

            uint8_t bcd = bin2bcd(hour);

            uint8_t mask = 0b00001111;
    
            uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30;    //mask the first 4bit
            uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  //mask the second 4bit and add value to go to the 0-9 values in the ascii table
            currentPosHr = POS1;
            
            clearChar(dev,POS1,POSY);
            displayChar(dev, POS1, POSY, firstDigit);
            clearChar(dev,POS2, POSY);
            displayChar(dev, POS2, POSY, secondDigit);
            prevHr = hour;

        }

        
        
        if(minute < 10 && (minute != prevMin)){
            clearChar(dev, currentPosMin,POSY); //clear previous char
            displayChar(dev, currentPosMin, POSY, '0'); //set it to 0 so it doesnt look empty


            currentPosMin = POS4; //update location 
            clearChar(dev,currentPosMin,POSY);
            displayChar(dev, currentPosMin, POSY, minute + '0');
            prevMin = minute;
            currentPosMin = POS3;

        }
        else if(minute > 9 && (minute != prevMin)){

            uint8_t bcd = bin2bcd(minute);

            uint8_t mask = 0b00001111;
    
            uint8_t secondDigit = ((bcd >> 0) & mask) + 0x30;    //mask the first 4bit
            uint8_t firstDigit = ((bcd >> 4) & mask) + 0x30;  //mask the second 4bit and add value to go to the 0-9 values in the ascii table            
            currentPosMin = POS3;
            
            clearChar(dev,POS3,POSY);
            displayChar(dev, POS3, POSY, firstDigit);
            clearChar(dev,POS4, POSY);
            displayChar(dev, POS4, POSY, secondDigit);    
            prevMin = minute;
        
        }

        if(day != prevDay){
            updateDay(rtc);
            printf("updating (prev) day   %d %d\n",day, prevDay);
            prevDay = day;
            printf("updating day   %d %d\n",day, prevDay);
        }

        blinkChar(dev, POSCOL, POSY, ':');
        
        /* TEST FOR FLICKERING 
        //flickers everytime on rtc_get_time call
        */
        
        /*while(true){
            rtc_get_time(rtc, &time);
 
        }*/

    }    
    

    printf("Done.\n");
    return 0;
}

