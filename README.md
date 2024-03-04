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
 * toggle automatic brightness  
 * 12h clock (AM/PM)  
 * come up with some sort of easy-to-navigate menu for all those things   

## Still todo (maybe):  
 * add option for a stopwatch  
 * add option to set alarms on time and day basis (not supported by the driver yet)  
 * add option to start a timer in minutes and hour basis (requires alarms, not supported by driver yet)  
 * add toggle option to display the date    

## Controls:    
### Clock display  
 * up-button to switch to temperature display  
 * middle-button to set the time and day  
 * down-button switch to the configurations menu
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
### Configurations Menu   
 * up-button cycle through options, when at the end of all options it will confirm those changes and return to the clock display    
 * middle-button toggle selected option   
 * bottom-button not used   
## Known bug(s):  
 * when there is a 0 somewhere and when turning on the clock the second digit on the hour or minute side is not displayed correctly to fix it switch to the temperature display and switch back this will reset the clock display  


