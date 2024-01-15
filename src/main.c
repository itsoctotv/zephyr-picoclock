#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>

struct display_capabilities caps;
uint8_t buf[7] = {};
struct display_buffer_descriptor buf_desc = {
    .buf_size = sizeof(buf),
    .width = 4,
    .height = 7,
    .pitch = 4,
};

int main() {
    int ret;

    const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!dev) {
        printk("Display device not ready\n");
        return 0;
    }
    display_get_capabilities(dev, &caps);
    if (!(caps.supported_pixel_formats & PIXEL_FORMAT_MONO01)) {
        printk("Expected pixel format not supported\n");
        return 0;
    }

    ret = display_set_pixel_format(dev, PIXEL_FORMAT_MONO01);
    if (ret < 0) {
        printk("display_set_pixel_format failed: %d\n", ret);
    }

    // set buffer and write to display (hopefully)
    int x = 0;
    int y = 0;

    while (1) {
        // clears first 5x7 char
        memset(buf, 0x00, sizeof(buf));
        ret = display_write(dev, x, y, &buf_desc, buf);
        if (ret < 0) {
            printk("display_write failed: %d\n", ret);
        }
        // check if end of screen
        if (x == 22) {
            break;
        } else {
            x++; // clear one by one
        }
    }

    
    memset(buf, 0xff, sizeof(buf));

    y = 0;
    
    //1st pos
    x = 0;
    ret = display_write(dev, x, y, &buf_desc, buf);
    //2nd pos
    x = 5;
    ret = display_write(dev, x, y, &buf_desc, buf);

    //colon
    
    
    //3rd pos
    x = 13;
    ret = display_write(dev, x, y, &buf_desc, buf);
    //4th pos
    x = 18;
    ret = display_write(dev, x, y, &buf_desc, buf);

    printk("finished main\n");
    return 0;
}
