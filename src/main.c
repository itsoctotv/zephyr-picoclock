#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

#include "font.h"

struct display_capabilities caps;

uint8_t buf[7] = { };
struct display_buffer_descriptor buf_desc = {
    .buf_size = sizeof(buf),
    .width = 8,
    .height = 7,
    .pitch = 8,
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
    int character = 0;
    while(1){
        k_msleep(150);
        for(int i = 0; i < sizeof(FONT[0]); i++){
                
            //copy current char to buffer
            buf[i] = FONT[character][i];
        }

        //print array contents for debug
        printf("buffer contents\n");
        for(int i = 0; i < sizeof(buf); i++){
            printf("0x%x\n",buf[i]);
        }

        for(int i = 0; i < buf_desc.height; i++){  //use buf_desc.height instead of caps.y_res to prevent potential buffer overflow
        
        //shift the end bits 4 places to the left, see font.h for reference

            buf[i] <<= 4;
        }

        //write current buffer/char to display
        
        int ret = display_write(dev, x, y, &buf_desc, buf);
        x += 5;
        character++;
        if(ret != 0){
            break; //just for testing
        }
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
