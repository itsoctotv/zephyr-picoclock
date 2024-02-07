#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
//#include <zephyr/drivers/adc.h>
//#include <zephyr/drivers/led.h>



#include "font.h"

//defines for main positions on the led matrix
#define POS1        0
#define POS2        5
#define POSCOL      9
#define POS3       13
#define POS4       18
#define POSY        0


int displayChar(const struct device *dev, int x, int y, char c);
int readChar(char c);
int clearChar(const struct device *dev, int index, int y);
int clearDisplay(const struct device *dev);
void blinkChar(const struct device *dev, int x, int y, char c);
void updateTemp(const struct device *dev);
int switchToTemp(const struct device *display_device,const struct device *temp_device);
void clearLEDs(const struct device *dev, int state);


void scrollText(const struct device *dev, char c);
void displayString(const struct device *dev, int x, int y, char* s);

void updateDay(const struct device *rtc);

int setTimeAndDate(const struct device *rtc);

struct display_capabilities caps;

const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);
const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios);





int main() {


    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(clock_rtc));

    const struct device *devtemp = DEVICE_DT_GET(DT_NODELABEL(clock_dts));


    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_configure_dt(&button2, GPIO_INPUT);
    gpio_pin_configure_dt(&button3, GPIO_INPUT);
    
    struct rtc_time time;/* = {
        .tm_sec = 0,            //sec
        .tm_min = 54,           //minute
        .tm_hour = 10,          //hour
        .tm_mday = 31,          //day of month
        .tm_mon = 0,            //month
        .tm_year = 2024,        //year
        .tm_wday = 3,           //weekday
        .tm_yday = 31,          //yearday
        .tm_isdst = -1,         //daylight saving flag
        .tm_nsec = 1            //nanosec
    };
  
    
    rtc_set_time(rtc,&time);
   */



    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01 );
    
   
    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);
    printf("size font: %d   %d   %d\n", sizeof(FONT), sizeof(FONT[0]), (sizeof(FONT) / sizeof(FONT[0])) );
    //printf("read font: %d\n", readChar('A'));

    clearDisplay(dev);

    //writing stuff
    
    displayChar(dev, POS1, POSY,'0');
    displayChar(dev, POS2, POSY,'1');
    displayChar(dev, POSCOL, POSY,':');
    displayChar(dev, POS3, POSY,2);
    displayChar(dev, POS4, POSY,3);    
    
    int hour,minute,seconds,day,currentPosHr, currentPosMin = 0;
    
    clearDisplay(dev);
    
    int prevHr,prevMin,prevDay = -1;
    int returnSwitchTemp = -1;
    
    printf("Readchar: %d\n%d\n%d\n\n", FONT[readChar('0')][0],FONT[readChar('1')][0],FONT[readChar('2')][0]);
    

    printf("bcd test\n");
    
    uint8_t val = 10;
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
    uint8_t maskFirstDigit = 0b11110000;
    uint8_t maskSecondDigit = 0b00001111;
    
    uint8_t firstDigit = bcd & maskFirstDigit;    //mask the first 4bit
    uint8_t secondDigit = bcd & maskSecondDigit;  //mask the second 4bit
    
    firstDigit += 0x30; //add value to go to the 0-9 values in the ascii table
    secondDigit += 0x30;

    printf("first digit ascii num: %x char: %c\n", firstDigit,(char)firstDigit);
    printf("second digit ascii num: %x char: %c\n", secondDigit,(char)secondDigit);
    
    for(;;);
    
    while(true){
        //printf("H: %d  M: %d  S: %d  \n",time.tm_hour, time.tm_min, time.tm_sec);
        
        //seconds = time.tm_sec;

        //button test

        //because of blinkChar() you'll have to wait until this if condition is checked in the while loop
        //just a simple example/test for the button; possible fix with "async interrupt handlers (?)"
        //https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
        //for now press the button longer a bit to trigger the if statements in main() and switchToTemp() 

        
        int val = gpio_pin_get_dt(&button);
        int val2 = gpio_pin_get_dt(&button2);
        int val3 = gpio_pin_get_dt(&button3);
        //printf("buttonval: %d\n", val);
        if(val != 0){
            
            returnSwitchTemp = switchToTemp(dev,devtemp); //switch to the temperature view and stay there until button is pressed again
            
        }
        if(val2 != 0){
            //scrollText(dev,':');

            //set time and date (wip)
            setTimeAndDate(rtc);
        }

        if(val3 != 0){
            //displayString(dev, POS1, POSY, "ABC");
            //quick update day when pressing btn3
            updateDay(rtc);
            clearLEDs(dev, 1);
            k_msleep(500);
            clearLEDs(dev, 0);
            printf("end\n");    
        }
        
        if(returnSwitchTemp == 0){
            //if returned from switchToTemp() update/set hours and minutes
            prevHr = -1;
            prevMin = -1; //set to default to simulate "restart"
            prevDay = -1;
            returnSwitchTemp = -1; //set it to -1 or any value to not trigger this statement again
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
//seperate 2digit int into 2 seperate ints https://www.log2base2.com/c-examples/loop/split-a-number-into-digits-in-c.html
        else if(hour > 9 && (hour != prevHr)){
            int num = hour;
            int secondDigit, firstDigit = 0;
            secondDigit = num % 10;
            firstDigit = num / 10;
            printf("HOUR %d %d\n", firstDigit, secondDigit);

            currentPosHr = POS1;
            
            clearChar(dev,POS1,POSY);
            displayChar(dev, POS1, POSY, firstDigit + '0');
            clearChar(dev,POS2, POSY);
            displayChar(dev, POS2, POSY, secondDigit + '0');
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
            int num = minute;
            int secondDigit, firstDigit = 0;
            secondDigit = num % 10;
            firstDigit = num / 10;
            printf("MINUTE %d %d\n", firstDigit, secondDigit);

            currentPosMin = POS3;
            
            clearChar(dev,POS3,POSY);
            displayChar(dev, POS3, POSY, firstDigit + '0');
            clearChar(dev,POS4, POSY);
            displayChar(dev, POS4, POSY, secondDigit + '0');    
            prevMin = minute;
        
        }

        if(day != prevDay){
            updateDay(rtc);
            printf("updating (prev) day   %d %d\n",day, prevDay);
            prevDay = day;
            printf("updating day   %d %d\n",day, prevDay);
        }
        

        //workaround (fix it)
        //k_msleep(500);    
        //fix it with rtc alarm or similar instead of delays

        

        blinkChar(dev, POSCOL, POSY, ':');
        /* TEST FOR FLICKERING 
        //flickers everytime on rtc_get_time call
        while(true){
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
        
    uint8_t dispbuf[22] = { };
    struct display_buffer_descriptor disp_buf_desc = {

        .buf_size = sizeof(dispbuf),
        .width = 22,
        .height = 7,
        .pitch = 22,
    };
     //clear the screen use whole-screen buffer
    memset(dispbuf, 0b00000000, sizeof(dispbuf));
    int ret = display_write(dev, 0, 0, &disp_buf_desc, dispbuf);
    if(ret != 0){
        printf("display_write failed\n");
        return -1;
    }
    else{
        return 0;
    }
}

int displayChar(const struct device *dev, int x, int y, char c){
    //read the char from FONT
    int index = readChar(c);
    if(index == -1){
        //nothing found, abort
        printf("no char was found! \n");
        return -1;
    }

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
int readChar(char c){
    return c;
    
    
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

    int currentPosVal1 = POS1;
    int currentPosVal2 = POS3;

    const struct gpio_dt_spec celsius = GPIO_DT_SPEC_GET(DT_ALIAS(led23),gpios);
    const struct gpio_dt_spec fahrenheit = GPIO_DT_SPEC_GET(DT_ALIAS(led22),gpios);

    gpio_pin_configure_dt(&celsius, GPIO_OUTPUT);
    gpio_pin_configure_dt(&fahrenheit, GPIO_OUTPUT);
    gpio_pin_set_dt(&celsius, 1); //turn on °C LED
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
            
            int num1 = tempFormatted1;
                        
            int secondDigit, firstDigit = 0;
            secondDigit = num1 % 10;
            firstDigit = num1 / 10;
            printf("TEMPVAL1 %d %d\n", firstDigit, secondDigit);
            currentPosVal1 = POS1;
            
            clearChar(display_device,POS1,POSY);
            displayChar(display_device, POS1, POSY, firstDigit + '0');
            clearChar(display_device,POS2, POSY);
            displayChar(display_device, POS2, POSY, secondDigit + '0');    
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
                    
            int num2 = tempFormatted2;
            
            int secondDigit, firstDigit = 0;
            secondDigit = num2 % 10;
            firstDigit = num2 / 10;
            printf("TEMPVAL2 %d %d\n", firstDigit, secondDigit);
            currentPosVal2 = POS3;
            clearChar(display_device,POS3,POSY);
            displayChar(display_device, POS3, POSY, firstDigit + '0');
            clearChar(display_device,POS4, POSY);
            displayChar(display_device, POS4, POSY, secondDigit + '0');    
        }
        //draw the dot in the middle and the 'C'

        //draw dot
        clearChar(display_device, POSCOL, POSY);
        displayChar(display_device, POSCOL, POSY, '.');

        //draw C
        //clearChar(display_device, POS4, POSY);
        //displayChar(display_device, POS4, POSY, 'C'); 

        k_msleep(500);//wait a bit 
        
        int val = gpio_pin_get_dt(&button);
        if(val != 0){
            printf("exit\n");
            gpio_pin_set_dt(&celsius, 0); //turn off LED when exiting
            gpio_pin_set_dt(&fahrenheit, 0);
            break;
        }
        int val2 = gpio_pin_get_dt(&button2);
        if(val2 != 0){
            if(isFahrenheit){
                printf("switching back to C\n");
                gpio_pin_set_dt(&fahrenheit, 0);
                gpio_pin_set_dt(&celsius, 1);
                isFahrenheit = false;
            }
            else{
                
                printf("switching to fahrenheit\n");
                gpio_pin_set_dt(&celsius, 0); //turn off C-LED

                gpio_pin_set_dt(&fahrenheit, 1);
                isFahrenheit = true;
            }
        }
        
    }
    clearDisplay(display_device);
    return 0;
    
    
}


void clearLEDs(const struct device *dev, int state){

    

    const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0),gpios);
    const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(DT_ALIAS(led1),gpios);
    const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(DT_ALIAS(led2),gpios);
    const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led3),gpios);
    const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(DT_ALIAS(led4),gpios);
    const struct gpio_dt_spec led5 = GPIO_DT_SPEC_GET(DT_ALIAS(led5),gpios);
    const struct gpio_dt_spec led6 = GPIO_DT_SPEC_GET(DT_ALIAS(led6),gpios);
    const struct gpio_dt_spec led7 = GPIO_DT_SPEC_GET(DT_ALIAS(led7),gpios);
    const struct gpio_dt_spec led8 = GPIO_DT_SPEC_GET(DT_ALIAS(led8),gpios);
    const struct gpio_dt_spec led9 = GPIO_DT_SPEC_GET(DT_ALIAS(led9),gpios);
    const struct gpio_dt_spec led10 = GPIO_DT_SPEC_GET(DT_ALIAS(led10),gpios);
    const struct gpio_dt_spec led11 = GPIO_DT_SPEC_GET(DT_ALIAS(led11),gpios);
    const struct gpio_dt_spec led12 = GPIO_DT_SPEC_GET(DT_ALIAS(led12),gpios);
    const struct gpio_dt_spec led13 = GPIO_DT_SPEC_GET(DT_ALIAS(led13),gpios);
    const struct gpio_dt_spec led14 = GPIO_DT_SPEC_GET(DT_ALIAS(led14),gpios);
    const struct gpio_dt_spec led15 = GPIO_DT_SPEC_GET(DT_ALIAS(led15),gpios);
    const struct gpio_dt_spec led16 = GPIO_DT_SPEC_GET(DT_ALIAS(led16),gpios);
    const struct gpio_dt_spec led17 = GPIO_DT_SPEC_GET(DT_ALIAS(led17),gpios);
    const struct gpio_dt_spec led18 = GPIO_DT_SPEC_GET(DT_ALIAS(led18),gpios);
    const struct gpio_dt_spec led19 = GPIO_DT_SPEC_GET(DT_ALIAS(led19),gpios);
    const struct gpio_dt_spec led20 = GPIO_DT_SPEC_GET(DT_ALIAS(led20),gpios);
    const struct gpio_dt_spec led21 = GPIO_DT_SPEC_GET(DT_ALIAS(led21),gpios);
    const struct gpio_dt_spec led22 = GPIO_DT_SPEC_GET(DT_ALIAS(led22),gpios);
    const struct gpio_dt_spec led23 = GPIO_DT_SPEC_GET(DT_ALIAS(led23),gpios);
    const struct gpio_dt_spec led24 = GPIO_DT_SPEC_GET(DT_ALIAS(led24),gpios);
    const struct gpio_dt_spec led25 = GPIO_DT_SPEC_GET(DT_ALIAS(led25),gpios);
    const struct gpio_dt_spec led26 = GPIO_DT_SPEC_GET(DT_ALIAS(led26),gpios);
    const struct gpio_dt_spec led27 = GPIO_DT_SPEC_GET(DT_ALIAS(led27),gpios);
    const struct gpio_dt_spec led28 = GPIO_DT_SPEC_GET(DT_ALIAS(led28),gpios);
    const struct gpio_dt_spec led29 = GPIO_DT_SPEC_GET(DT_ALIAS(led29),gpios);
    const struct gpio_dt_spec led30 = GPIO_DT_SPEC_GET(DT_ALIAS(led30),gpios);
    const struct gpio_dt_spec led31 = GPIO_DT_SPEC_GET(DT_ALIAS(led31),gpios);

    gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led3, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led4, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led5, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led6, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led7, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led8, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led9, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led10, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led11, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led12, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led13, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led14, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led15, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led16, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led17, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led18, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led19, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led20, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led21, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led22, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led23, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led24, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led25, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led26, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led27, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led28, GPIO_OUTPUT);
    gpio_pin_configure_dt(&led29, GPIO_OUTPUT);    
    gpio_pin_configure_dt(&led30, GPIO_OUTPUT);    
    gpio_pin_configure_dt(&led31, GPIO_OUTPUT); 


    gpio_pin_set_dt(&led0, state);
    gpio_pin_set_dt(&led1, state);
    gpio_pin_set_dt(&led2, state);
    gpio_pin_set_dt(&led3, state);
    gpio_pin_set_dt(&led4, state);
    gpio_pin_set_dt(&led5, state);
    gpio_pin_set_dt(&led6, state);
    gpio_pin_set_dt(&led7, state);
    gpio_pin_set_dt(&led8, state);
    gpio_pin_set_dt(&led9, state);
    gpio_pin_set_dt(&led10, state);
    gpio_pin_set_dt(&led11, state);
    gpio_pin_set_dt(&led12, state);
    gpio_pin_set_dt(&led13, state);
    gpio_pin_set_dt(&led14, state);
    gpio_pin_set_dt(&led15, state);
    gpio_pin_set_dt(&led16, state);
    gpio_pin_set_dt(&led17, state);
    gpio_pin_set_dt(&led18, state);
    gpio_pin_set_dt(&led19, state);
    gpio_pin_set_dt(&led20, state);
    gpio_pin_set_dt(&led21, state);
    gpio_pin_set_dt(&led22, state);
    gpio_pin_set_dt(&led23, state);
    gpio_pin_set_dt(&led24, state);
    gpio_pin_set_dt(&led25, state);
    gpio_pin_set_dt(&led26, state);
    gpio_pin_set_dt(&led27, state);
    gpio_pin_set_dt(&led28, state);
    gpio_pin_set_dt(&led29, state);    
    gpio_pin_set_dt(&led30, state);    
    gpio_pin_set_dt(&led31, state);   
    
}



//[WIP]
void displayString(const struct device *dev, int x, int y, char* s){
    printf("inside displaystring thing\n");
    k_msleep(500);
    int pos = x;
    clearDisplay(dev);
    for(int i = 0; i < sizeof(s); i++){
        displayChar(dev, pos+5, y, s[i]); //goto next position for next char
        
    }
    while(true){
        
        int val = gpio_pin_get_dt(&button3);
        if(val != 0){
            printf("exit\n");
            clearDisplay(dev);
            return;
        }
        printf("waiting for input\n");
        k_msleep(500);
    }
}

void scrollText(const struct device *dev, char c){
    clearDisplay(dev);

    char singleLineBuff[1] = { };
    memset(singleLineBuff, 0x00, sizeof(singleLineBuff));
    struct display_buffer_descriptor char_buf_desc = {
            .buf_size = sizeof(singleLineBuff),
            .width =  1,
            .height = 7,
            .pitch =  1,
        };
    while(true){
        for(int i = 0; i < 18; i++){
            displayChar(dev, i+1, POSY, c);
            display_write(dev, i, POSY, &char_buf_desc, singleLineBuff); //delete a single line
            k_msleep(100);

            
            
        }
        for(int i = 19; i > 0; i--){
            displayChar(dev, i-1, POSY, c);
            display_write(dev, i+3, POSY, &char_buf_desc, singleLineBuff);
            k_msleep(100);
            
        }

        int val = gpio_pin_get_dt(&button2);
        if(val != 0){
            printf("exit\n");
            break;
        }
    
    }
    clearDisplay(dev);
    
}
void updateDay(const struct device *rtc){
    struct rtc_time currTime;

    //maybe there is a better way?
    //monday leds
    const struct gpio_dt_spec monday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led2),gpios);
    const struct gpio_dt_spec monday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led3),gpios);

    gpio_pin_configure_dt(&monday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&monday1, GPIO_OUTPUT);

    //tuesday leds
    const struct gpio_dt_spec tuesday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led4),gpios);
    const struct gpio_dt_spec tuesday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led5),gpios);
    
    gpio_pin_configure_dt(&tuesday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&tuesday1, GPIO_OUTPUT);

    //wednesday leds
    const struct gpio_dt_spec wednesday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led6),gpios);
    const struct gpio_dt_spec wednesday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led7),gpios);

    gpio_pin_configure_dt(&wednesday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&wednesday1, GPIO_OUTPUT);

    //thursday leds
    const struct gpio_dt_spec thursday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led8),gpios);
    const struct gpio_dt_spec thursday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led9),gpios);
    
    gpio_pin_configure_dt(&thursday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&thursday1, GPIO_OUTPUT);

    //friday leds
    const struct gpio_dt_spec friday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led10),gpios);
    const struct gpio_dt_spec friday1  = GPIO_DT_SPEC_GET(DT_ALIAS(led11),gpios);

    gpio_pin_configure_dt(&friday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&friday1, GPIO_OUTPUT);

    //saturday leds
    const struct gpio_dt_spec saturday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led12),gpios);
    const struct gpio_dt_spec saturday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led13),gpios);
    
    gpio_pin_configure_dt(&saturday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&saturday1, GPIO_OUTPUT);

    //sunday leds
    const struct gpio_dt_spec sunday0 = GPIO_DT_SPEC_GET(DT_ALIAS(led14),gpios);
    const struct gpio_dt_spec sunday1 = GPIO_DT_SPEC_GET(DT_ALIAS(led16),gpios);
    
    gpio_pin_configure_dt(&sunday0, GPIO_OUTPUT);
    gpio_pin_configure_dt(&sunday1, GPIO_OUTPUT);


    //turn off all first (clears the previous day)


    gpio_pin_set_dt(&sunday0, 0);
    gpio_pin_set_dt(&sunday1, 0);
    gpio_pin_set_dt(&monday0, 0);
    gpio_pin_set_dt(&monday1, 0);
    gpio_pin_set_dt(&tuesday0, 0);
    gpio_pin_set_dt(&tuesday1, 0);
    gpio_pin_set_dt(&wednesday0, 0);
    gpio_pin_set_dt(&wednesday1, 0);
    gpio_pin_set_dt(&thursday0, 0);
    gpio_pin_set_dt(&thursday1, 0);
    gpio_pin_set_dt(&friday0, 0);
    gpio_pin_set_dt(&friday1, 0);
    gpio_pin_set_dt(&saturday0, 0);
    gpio_pin_set_dt(&saturday1, 0);



    rtc_get_time(rtc, &currTime);
    printf("currTime: %d\n",currTime.tm_wday);
    switch(currTime.tm_wday){
        case 0:
            printf("Sunday\n");
            gpio_pin_set_dt(&sunday0, 1);
            gpio_pin_set_dt(&sunday1, 1);
            break;
        case 1:
            printf("Monday\n");
            gpio_pin_set_dt(&monday0, 1);
            gpio_pin_set_dt(&monday1, 1);
            break;
        case 2:
            printf("Tuesday\n");
            gpio_pin_set_dt(&tuesday0, 1);
            gpio_pin_set_dt(&tuesday1, 1);
            break;
        case 3:
            printf("Wednesday\n");
            gpio_pin_set_dt(&wednesday0, 1);
            gpio_pin_set_dt(&wednesday1, 1);
            break;
        case 4:
            printf("Thursday\n");
            gpio_pin_set_dt(&thursday0, 1);
            gpio_pin_set_dt(&thursday1, 1);
            break;
        case 5:
            printf("Friday\n");
            gpio_pin_set_dt(&friday0, 1);
            gpio_pin_set_dt(&friday1, 1);
            break;
        case 6:
            printf("Saturday\n");
            gpio_pin_set_dt(&saturday0, 1);
            gpio_pin_set_dt(&saturday1, 1);
            break;
        default:
            printf("Failed to read Weekdays!\n");
            
    }
}

int setTimeAndDate(const struct device *rtc){

    printf("Set time; in format: HH:MM:SS\n\n\n");

    


    printf("exiting\n");
    
    

    return 0; 
}
