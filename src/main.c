#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

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


struct display_capabilities caps;




 
int main() {

    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    
    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01 );
    
   
    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);
    printf("size font: %d   %d   %d\n", sizeof(FONT), sizeof(FONT[0]), (sizeof(FONT) / sizeof(FONT[0])) );
    printf("read font: %d\n", readChar('A'));

    clearDisplay(dev);

    //writing stuff
    
    displayChar(dev, POS1, POSY,'6');
    displayChar(dev, POS2, POSY,'7');
    displayChar(dev, POSCOL, POSY,':');
    displayChar(dev, POS3, POSY,'8');
    displayChar(dev, POS4, POSY,'9');    
    
    //blinking colon
    while(true){
        displayChar(dev, POSCOL, POSY, ':');
        k_msleep(500);
        clearChar(dev, POSCOL, POSY);      
        k_msleep(500);
        
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
