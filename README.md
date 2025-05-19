# Raspberry PI adventures in C / Python
 
## Raspberry Pi Time and Temperature Display with LCD

This Raspberry Pi project displays the current time and local temperature on an LCD screen, making use of network connectivity for real-time accuracy. The system synchronizes time using the Network Time Protocol (NTP) and fetches current weather data from the OpenWeather API.

### Key Features:
 - Real-Time Clock: Time is synchronized via NTP, ensuring it stays accurate even after power cycles or reboots.

 - Live Temperature Updates: The Raspberry Pi queries the OpenWeather API at regular intervals to fetch the current temperature for a specified location (my home office in the example).

 - LCD Display Output: A 16x2 LCD, with I2C interface, is used to display the time and temperature in a readable format.

 - Automatic Refresh: Both time and temperature are updated automatically, with customizable intervals.

### Tools & Technologies:
 - Raspberry Pi (any model with internet access)

 - Python (working) and C (trying to make it to work using [uLibC](https://uclibc.org/))
   - The goal is to use a RPI already running [piCorePlayer](https://www.picoreplayer.org/) 

 - LCD display (I2C compatible)

 - NTP synchronization

 - requests for OpenWeather API calls

 - OpenWeatherMap API key

### Use Case:
Ideal for home offices, kitchens, or maker spaces where you want a small display that shows both the current time and weather conditions at a glance.
