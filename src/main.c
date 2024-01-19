
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

#include "font.h"

int displayChar(const struct device *dev, int x, int y, char c);
int readChar(char c);


struct display_capabilities caps;



uint8_t dispbuf[22] = { };
struct display_buffer_descriptor disp_buf_desc = {
    .buf_size = sizeof(dispbuf),
    .width = 22,
    .height = 7,
    .pitch = 22,
};


 
int main() {

    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    
    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01 );
    
   
    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);
    printf("size font: %d   %d   %d\n", sizeof(FONT), sizeof(FONT[0]), (sizeof(FONT) / sizeof(FONT[0])) );
    printf("read font: %d\n", readChar('A'));

    //clear the screen use whole-screen buffer

    memset(dispbuf, 0b00000000, sizeof(dispbuf));

    display_write(dev, 0, 0, &disp_buf_desc, dispbuf);

    int ret = displayChar(dev, 0, 0, 'A');
    if(ret != 0){
        printf("displayChar() failed\n");
    }
    
    

    printf("Done.\n");
    return 0;
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

    for(int i = 0; i < sizeof(charbuf); i++){
        printf("%x\n", charbuf[i]);
    }
    printf("here\n");
    //---skip first value in FONT because its the identifier---
    int i = 1;
    int j = 2;
    int k = 0;
    while(true){
        unsigned char firstLine =  FONT[index][i]; //6
        unsigned char secondLine = FONT[index][j]; //9
        
        unsigned char mergedLine = firstLine<<4 | secondLine;
        charbuf[k] = mergedLine;
        i+=2;
        if(j == 6){
            j++;
        }
        else{
            j+=2;
        }
        
        k_msleep(800);
        int ret = display_write(dev, x, y, &char_buf_desc, charbuf);
        if(ret != 0){
            break;
        }
        k++;    
    }
   
    
    
    printf("here end\n");
    return 0;

/*
    for(int i = 0; i < sizeof(charbuf); i++){
        for(int j = 0; j < sizeof(charbuf); j++){
            if(charbuf[j] == 0b00000000){
                //swap
                char temp = charbuf[j];
                charbuf[j] = charbuf[j+1];
                charbuf[j+1] = temp;
                
            }
        }
    }*/

    //rearranges the buffer because there are '0's between each element from shifting and merging 
    /*int i,j = 0;
    
    //two 4bit values in one 8bit value
    //==> 4x 8bit values are enough for the hole character the rest is undefined behaviour 
    for(i = 0; i < sizeof(charbuf); i++){
        if(charbuf[i] != 0b00000000){
            charbuf[j++] = charbuf[i];
        }
    }*/

    

    int ret = display_write(dev, x, y, &char_buf_desc, charbuf);
    if(ret != 0){
        printf("Failed display_write\n");
        return -1; //failed to display

    }
    else{
        
        return 0; //success        
    }
    
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
