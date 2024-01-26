#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

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

struct display_capabilities caps;

const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);


int main() {

    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    const struct device *rtc = DEVICE_DT_GET(DT_NODELABEL(clock_rtc));

    const struct device *devtemp = DEVICE_DT_GET(DT_NODELABEL(clock_dts));
    

    gpio_pin_configure_dt(&button, GPIO_INPUT);
    
    

    struct rtc_time time = {
        .tm_sec = 0,
        .tm_min = 21,
        .tm_hour = 8,
        .tm_mday = 1,
        .tm_mon = 1,
        .tm_year = 2024,
        .tm_wday = 1,
        .tm_yday = 1,
        .tm_isdst = -1,
        .tm_nsec = 1
    };

    
    
    rtc_set_time(rtc,&time);

    
    
    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01 );
    
   
    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);
    printf("size font: %d   %d   %d\n", sizeof(FONT), sizeof(FONT[0]), (sizeof(FONT) / sizeof(FONT[0])) );
    printf("read font: %d\n", readChar('A'));

    clearDisplay(dev);

    //writing stuff
    
    displayChar(dev, POS1, POSY,'A');
    displayChar(dev, POS2, POSY,'B');
    displayChar(dev, POSCOL, POSY,':');
    displayChar(dev, POS3, POSY,1);
    displayChar(dev, POS4, POSY,2);    

    int hour,minute,seconds,currentPosHr, currentPosMin = 0;
    
    clearDisplay(dev);
    
    int prevHr,prevMin = -1;
    int returnSwitchTemp = -1;

    while(true){
        //printf("H: %d  M: %d  S: %d  \n",time.tm_hour, time.tm_min, time.tm_sec);
        
        //seconds = time.tm_sec;

        //button test

        //because of blinkChar() you'll have to wait until this if condition is checked in the while loop
        //just a simple example/test for the button; possible fix with "async interrupt handlers (?)"
        //https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
        //for now press the button longer a bit to trigger the if statements in main() and switchToTemp() 

        
        int val = gpio_pin_get_dt(&button);
        //printf("buttonval: %d\n", val);
        if(val != 0){
           
            returnSwitchTemp = switchToTemp(dev,devtemp); //switch to the temperature view and stay there until button is pressed again
            
        }

        if(returnSwitchTemp == 0){
            //if returned from switchToTemp() update/set hours and minutes
            prevHr = -1;
            prevMin = -1; //set to default to simulate "restart"
            returnSwitchTemp = -1; //set it to -1 or any value to not trigger this statement again
        }


        
            
        rtc_get_time(rtc, &time);
        

        hour = time.tm_hour;       
        minute = time.tm_min; 

        
        
        if(hour < 10 && (hour != prevHr)){
            clearChar(dev,currentPosHr,POSY); //clear previous char
            displayChar(dev, currentPosHr, POSY, 0); //set it to 0 so it doesnt look empty
            currentPosHr = POS2; //update location 
            clearChar(dev,currentPosHr,POSY);
            displayChar(dev, currentPosHr, POSY, hour);
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
            displayChar(dev, POS1, POSY, firstDigit);
            clearChar(dev,POS2, POSY);
            displayChar(dev, POS2, POSY, secondDigit);
            prevHr = hour;

        }


        
        if(minute < 10 && (minute != prevMin)){
            clearChar(dev, currentPosMin,POSY); //clear previous char
            displayChar(dev, currentPosMin, POSY, 0); //set it to 0 so it doesnt look empty


            currentPosMin = POS4; //update location 
            clearChar(dev,currentPosMin,POSY);
            displayChar(dev, currentPosMin, POSY, minute);
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
            displayChar(dev, POS3, POSY, firstDigit);
            clearChar(dev,POS4, POSY);
            displayChar(dev, POS4, POSY, secondDigit);    
            prevMin = minute;
        
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

        //possible fix with rtc alarms 

        
        

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

    int indexFirstLine = 1;
    int indexSecondLine = 2;
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
    int totalRows = sizeof(FONT) / sizeof(FONT[0]);
    for(int i = 0; i < totalRows; i++){
        if(FONT[i][0] == c){
            return i; //return index inside FONT[];
        }
        
    }
    return -1; //nothing found
    
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

    while(true){
        struct sensor_value temp;
        sensor_sample_fetch(temp_device);
        sensor_channel_get(temp_device, SENSOR_CHAN_AMBIENT_TEMP, &temp);

        printf("%d.%d°C\n",temp.val1,temp.val2);
        


        //first value of temp
        if(temp.val1 < 10){
            clearChar(display_device,currentPosVal1,POSY); //clear previous char
           
            currentPosVal1 = POS2; //update location 
            clearChar(display_device,currentPosVal1,POSY);
            displayChar(display_device, currentPosVal1, POSY, temp.val1);
           
            currentPosVal1 = POS1;
        
        }
        
        else if(temp.val1 > 9){
        //if temp.val1 is double digit
            
            int num1 = temp.val1;
                        
            int secondDigit, firstDigit = 0;
            secondDigit = num1 % 10;
            firstDigit = num1 / 10;
            printf("TEMPVAL1 %d %d\n", firstDigit, secondDigit);
            currentPosVal1 = POS1;
            
            clearChar(display_device,POS1,POSY);
            displayChar(display_device, POS1, POSY, firstDigit);
            clearChar(display_device,POS2, POSY);
            displayChar(display_device, POS2, POSY, secondDigit);    
        }

        
        //second value of temp only 
        if(temp.val2 > -1){ // only single digit; if statement is basically useless

            currentPosVal2 = POS3;
            int val = temp.val2; 
            printf("val: %d\n");
            while(val >= 10){
                
                val = val / 10; //only get the first digit
            }
            printf("val2: %d\n",val);
            clearChar(display_device,currentPosVal2,POSY);
            displayChar(display_device, currentPosVal2, POSY, val);
                   
        }
        //draw the dot in the middle and the 'C'

        //draw dot
        clearChar(display_device, POSCOL, POSY);
        displayChar(display_device, POSCOL, POSY, '.');

        //draw C
        clearChar(display_device, POS4, POSY);
        displayChar(display_device, POS4, POSY, 'C'); 

        k_msleep(500);//wait a bit 
        
        int val = gpio_pin_get_dt(&button);
        if(val != 0){
            printf("exit\n");
            break;
        }
        
    }
    clearDisplay(display_device);
    return 0;
    
    
}

