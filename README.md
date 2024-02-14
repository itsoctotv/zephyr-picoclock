# Zephyr Pico Clock Sample   
---   
[WIP] Sample on displaying characters and more on the [Waveshare Raspberry Pi Pico Clock](https://www.waveshare.com/wiki/Pico-Clock-Green) with [Zephyr](https://github.com/zephyrproject-rtos/zephyr) and the Display API. 

---
## Features:  
 * 24h clock  
 * day display  
 * temperature display in Celsius and Fahrenheit  
 * automatic brightness adjustment  
 * set time and day directly from the clock  
 * displaying chars via bitmap fonts  

## Still todo (maybe):  
 * 12h clock (AM/PM)  
 * toggle automatic brightness  
 * add option for a stopwatch  
 * add option to set alarms on time and day basis (not supported by the driver yet)  
 * add option to start a timer in minutes and hour basis (requires alarms, not supported by driver yet)  
 * add toggle option to display the date    
 * come up with some sort of easy-to-navigate menu for all those things   

## Controls:    
### Clock display  
 * up-button to switch to temperature display  
 * middle-button to set the time and day  
 * down-button (not in use, for testing purposes)  
### Temperature display  
 * up-button to switch back to the clock display  
 * middle-button to toggle between Celsius and Fahrenheit  
 * down-button not used  
### Set time and day display  
 * up-button to set values in positive direction
 * middle-button to confirm current value and go to the next,   
   if hours, minutes and day is set press again to write changes   
   to the RTC and switch back to the clock display   
 * bottom-button to set values in negative direction   
