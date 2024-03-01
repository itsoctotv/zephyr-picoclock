#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/adc.h>

#include "font.h"

//defines for main positions on the led matrix
#define POS1        0
#define POS2        5
#define POSCOL      9
#define POS3       13
#define POS4       18
#define POSY        0

int switchToMenu(const struct device *dev);

int displayChar(const struct device *dev, int x, int y, char c);
int clearChar(const struct device *dev, int index, int y);
int clearDisplay(const struct device *dev);
void blinkChar(const struct device *dev, int x, int y, char c);
void updateTemp(const struct device *dev);
int switchToTemp(const struct device *display_device,const struct device *temp_device);
void setLED(const struct device *dev, int number, int state);
void clearLEDs (const struct device *dev);
int dimmDisplay(const struct device *dev, uint8_t brightness);

void updateDay(const struct device *rtc);

void updateAutolight(const struct device *dev, const struct adc_dt_spec adc);

int setTimeAndDay(const struct device *dev, const struct device *rtc);

struct display_capabilities caps;

const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);


//basically copied example from zephyr/samples/drivers/adc
//with the rpi_pico.overlay thing because nothing else worked
//in boards/rpi_pico.overlay

const struct adc_dt_spec adc_channel0 = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(clock_rtc));

const struct device *devtemp = DEVICE_DT_GET(DT_NODELABEL(clock_dts));


//Flags
bool f_autolight = true;


int main() {


    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    
    struct rtc_time time;/* = {
        .tm_sec = 0,            //sec
        .tm_min = 47,           //minute
        .tm_hour = 9,          //hour
        .tm_mday = 13,          //day of month
        .tm_mon = 1,            //month
        .tm_year = 2024,        //year
        .tm_wday = 2,           //weekday
        .tm_yday = 44,          //yearday
        .tm_isdst = -1,         //daylight saving flag
        .tm_nsec = 1            //nanosec
    };
  
    
    rtc_set_time(rtc,&time);


*/

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
    int currentPosHr = 0;
    int currentPosMin = 0;
    
    clearDisplay(dev);
    
    int prevHr = -1;
    int prevMin = -1;
    int prevDay = -1;
    int returnSwitchTemp = -1;
    int returnSwitchMenu = -1;
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
            
            setTimeAndDay(dev,rtc);

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
            setLED(dev, 10, 1); //if you change to temp display or clockset display the led goes of because clearLEDs() is called in those functions this turns on the hole time while toggleAutolight is true

            updateAutolight(dev, adc_channel0);
            
        }
        else{
            setLED(dev, 10, 0);
        }
        
        
        if(returnSwitchTemp == 0){
            //if returned from switchToTemp() update/set hours and minutes
            prevHr = -1;
            prevMin = -1; //set to default to simulate "restart"
            prevDay = -1;
            returnSwitchTemp = -1; //set it to -1 or any value to not trigger this statement again
        }
        if(returnSwitchMenu == 0){
            prevHr = -1;
            prevMin = -1;
            prevDay = -1;
            returnSwitchMenu = -1;
        }

        
        rtc_get_time(rtc, &time);
        
        hour = time.tm_hour;       
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


int clearChar(const struct device *dev, int index, int y){
    uint8_t charbuf[7] = { };
    memset(charbuf, 0b00000000, sizeof(charbuf));
    struct display_buffer_descriptor char_buf_desc = {
        .buf_size = sizeof(charbuf),
        .width = 4,
        .height = 7,
        .pitch = 4,
    };
    int ret = display_write(dev, index, y, &char_buf_desc, charbuf);
    if(ret != 0){
        printf("display_write failed\n");
        
        return -1;
    }
    else{
        return 0;
    }
    
}

int clearDisplay(const struct device *dev){

    int x = 22;
    int y = 7;

    uint8_t dispbuf[22] = { };

    struct display_buffer_descriptor disp_buf_desc = {

        .buf_size = sizeof(dispbuf),
        .width = x,
        .height = y,
        .pitch = x,
    };
    memset(dispbuf, 0b00000000, sizeof(dispbuf)); 
     //clear the screen use whole-screen buffer
    int ret = display_write(dev, 0, 0, &disp_buf_desc, dispbuf);
    if(ret != 0){
        printf("display_write failed\n");
        return -1;
    }
    else{
        return 0;
    }
}

int displayChar(const struct device *dev, int x, int y, char c) {
    
    uint8_t index = c;

    uint8_t charbuf[7] = { };
    memset(charbuf, 0b00000000, sizeof(charbuf));
    struct display_buffer_descriptor char_buf_desc = {
        .buf_size = sizeof(charbuf),
        .width =  4,
        .height = 7,
        .pitch =  4,
    };

    //display chars use char buffer
            
    //shift 4bits to the left and use the right 4 bits as the second character
    //    0000 0110 <- normal values 
    //    0000 1001 
    //
    //    0110 0000 <- shifted 4 bits to the left
    //    bitwise OR ( | ) with the non shifted next value
    //    0110 0000
    //    0000 1001
    //    ---------
    //    0110 1001
    // 1. line|2. line (it's probably way too complicated than it should be)

    int indexFirstLine = 0;
    int indexSecondLine = 1;
    for(int i = 0; i < sizeof(charbuf); i++){
        unsigned char firstLine = FONT[index][indexFirstLine];
        unsigned char secondLine = FONT[index][indexSecondLine];
        unsigned char mergedLine = firstLine << 4 | secondLine; //shift first 4 places to the left and merge shifted first and non-shifted second
        charbuf[i] = mergedLine;
        indexFirstLine += 2; //count to the second next line, skip the next because it was merged from secondLine already 
        if(indexSecondLine == (sizeof(charbuf) - 1)){
            indexSecondLine++;  //if at end of array only get next line otherwise ==> array out of bounce 
        }
        else{
            indexSecondLine += 2;
        }
    }

    int ret = display_write(dev, x, y, &char_buf_desc, charbuf); //display buffer

    if(ret != 0){
        printf("display_write failed\n"); 
    }

    return 0;

    
}
void blinkChar(const struct device *dev, int x, int y, char c){
        //blinking colon
        displayChar(dev, x, y, c);
        k_msleep(500);
        clearChar(dev, x, y);      
        k_msleep(500);

}

void updateTemp(const struct device *dev){

    struct sensor_value temp;
    sensor_sample_fetch(dev);
    sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    printf("%d.%d\n",temp.val1,temp.val2);
}

int switchToTemp(const struct device *display_device, const struct device *temp_device){

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
            setLED(dev, 7, 0); //C //turn off LED when exiting
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
        
    }
    clearDisplay(display_device);
    return 0;
    
    
}

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
            struct gpio_dt_spec led26 = GPIO_DT_SPEC_GET(DT_ALIAS(led26),gpios);
            struct gpio_dt_spec led27 = GPIO_DT_SPEC_GET(DT_ALIAS(led27),gpios);
            gpio_pin_configure_dt(&led26, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led27, GPIO_OUTPUT);
            gpio_pin_set_dt(&led26, state);   
            gpio_pin_set_dt(&led27, state);   
            break;
        case 9:
            struct gpio_dt_spec led28 = GPIO_DT_SPEC_GET(DT_ALIAS(led28),gpios);
            struct gpio_dt_spec led29 = GPIO_DT_SPEC_GET(DT_ALIAS(led29),gpios);
            gpio_pin_configure_dt(&led28, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led29, GPIO_OUTPUT);
            gpio_pin_set_dt(&led28, state);
            gpio_pin_set_dt(&led29, state);
            break;
        case 10:
            struct gpio_dt_spec led30 = GPIO_DT_SPEC_GET(DT_ALIAS(led30),gpios);
            struct gpio_dt_spec led31 = GPIO_DT_SPEC_GET(DT_ALIAS(led31),gpios);
            gpio_pin_configure_dt(&led30, GPIO_OUTPUT);
            gpio_pin_configure_dt(&led31, GPIO_OUTPUT);
            gpio_pin_set_dt(&led30, state);
            gpio_pin_set_dt(&led31, state);
            break;
        case 11:
            struct gpio_dt_spec led24 = GPIO_DT_SPEC_GET(DT_ALIAS(led24),gpios);
            gpio_pin_configure_dt(&led24, GPIO_OUTPUT);
            gpio_pin_set_dt(&led24, state);
            break;
        case 12:
            struct gpio_dt_spec led25 = GPIO_DT_SPEC_GET(DT_ALIAS(led25),gpios);
            gpio_pin_configure_dt(&led25, GPIO_OUTPUT);
            gpio_pin_set_dt(&led25, state);
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

int dimmDisplay(const struct device *dev, uint8_t brightness){
    return display_set_brightness(dev,brightness);
    
}

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

int switchToMenu(const struct device *dev){
    clearDisplay(dev);

    //menu for toggling various options

    
    

    //autolight display test

    displayChar(dev, POS1, POSY, 'A');
    displayChar(dev, POS2, POSY, 'L');
    displayChar(dev, POSCOL, POSY, ':');
    k_msleep(250);
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

        int exitButton = gpio_pin_get_dt(&button3);
        if(exitButton != 0){
            printf("breaking here\n");
            break;
        }
        printf("current state autolight flag: %d\n", (uint8_t)f_autolight);
    }
    return 0;
    
        
}
