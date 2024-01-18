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
    .width =  4,
    .height = 7,
    .pitch =  4,
};

 
int main() {

    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    
    display_get_capabilities(dev, &caps);
    display_set_pixel_format(dev, PIXEL_FORMAT_MONO01);
    
    // set buffer and write to display (hopefully)
    int x = 0;
    int y = 0;
    int charindex = 1; //A

    printf("x res: %d\ny res: %d\n",caps.x_resolution, caps.y_resolution);

    //clear the screen
    for(int i = 0; i < caps.x_resolution; i++){
        
        memset(buf,0x00, sizeof(buf));
        display_write(dev, i, y, &buf_desc, buf);
    }
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
    // 1. line|2. line
    for(int i = 0; i < sizeof(FONT[0]); i+=2){
        
        buf[i] = (FONT[charindex][i] << 4) | FONT[charindex][i+1];
        
    }

    //rearranges the buffer because there are '0's between each element
    int i,j = 0;
    //loops only half of the buffer because it has two 4bit values in one 8bit value
    //==> 4x 8bit values are enough for the hole character the rest is undefined behaviour 
    for(i = 0; i < sizeof(buf); i++){
        if(buf[i] != 0b00000000){
            buf[j++] = buf[i];
        }
    }

    display_write(dev, x,y, &buf_desc, buf);
    display_write(dev, x+5,y, &buf_desc, buf);
    display_write(dev, x+10,y, &buf_desc, buf);
    display_write(dev, x+15,y, &buf_desc, buf);
        
    
    for(int i = 0; i < sizeof(buf); i++){
       
        printf("%x\n",buf[i]);
    }
    printf("Done.\n");
    return 0;
}
