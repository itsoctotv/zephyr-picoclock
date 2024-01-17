#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

#include "font.h"

struct display_capabilities caps;

uint8_t buf[7] = { };
uint8_t buf_colon[7] = { };
struct display_buffer_descriptor buf_desc = {
    .buf_size = sizeof(buf),
    .width =  8,
    .height = 7,
    .pitch =  8,
};

 
int main() {

    const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    
    display_get_capabilities(dev, &caps);
    

    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01);
    
    // set buffer and write to display (hopefully)
    int x = 0;
    int y = 0;

    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);

    //clear the screen
    for(int i = 0; i < caps.x_resolution; i++){
        
        memset(buf,0x00, sizeof(buf));
        display_write(dev, i, y, &buf_desc, buf);
    }
    //int character = 0;
           
   
    /*
    for(int i = 0; i < sizeof(FONT[0]); i++){
            
        //copy current char to buffer
        
        char res = (FONT[character][i] << 4 ) | FONT[character+1][i];
        printf("retru nval before: %x\n",res);
        char mask = 0b11111111;
        res = res & mask;
        printf("return val after: %x\n", res);
        buf[i] = res;
          
        }*/
        //first char
    for(int i = 0; i < buf_desc.height; i++){ 
    //use buf_desc.height instead of caps.y_res to prevent potential buffer overflow

        //shift the end bits 4 places to the left, see font.h for reference
        buf[i] = FONT[1][i] << 4;
            
                
    }
    display_write(dev, x, y, &buf_desc, buf);    

    x += 5;
    //second char
    for(int i = 0; i < buf_desc.height; i++){ 
    //use buf_desc.height instead of caps.y_res to prevent potential buffer overflow


       //shift the end bits 4 places to the left, see font.h for reference
       buf[i] = FONT[2][i] << 4;
            
                
    }
    display_write(dev, x, y, &buf_desc, buf);    
    x += 5; //skip the colon

    //--test--
    //third char
    for(int i = 0; i < buf_desc.height; i++){ 
    //use buf_desc.height instead of caps.y_res to prevent potential buffer overflow

        //shift the end bits 4 places to the left, see font.h for reference
        buf[i] = FONT[3][i] << 4;
            
                
    }
    display_write(dev, x, y, &buf_desc, buf);    
    x += 5;

    //last char
    for(int i = 0; i < buf_desc.height; i++){ 
    //use buf_desc.height instead of caps.y_res to prevent potential buffer overflow


       //shift the end bits 4 places to the left, see font.h for reference
       buf[i] = FONT[4][i] << 4;
            
                
    }
    display_write(dev, x, y, &buf_desc, buf);    


    while(1){
        for(int i = 0; i < sizeof(buf_colon); i++){
            buf_colon[i] = FONT[0][i];
            buf_colon[i] <<= 4;
            
        }
        display_write(dev, 9, y, &buf_desc, buf_colon);
        k_msleep(500);
        memset(buf_colon, 0x00, sizeof(buf_colon));
        display_write(dev, 9, y, &buf_desc, buf_colon);
        k_msleep(500);

    }    
    
    printf("finished main.\n");
    return 0;
}













    //fill buffer with first char in the FONT array
/*    int charstop = 0;
    for(int i = 0; i < sizeof(FONT); i++){
        if(i % 7 == 0){
            charstop = i;
            break;
        }
        else{
            
            buf[i] = FONT[i];
        }
    }
*/
