from RPLCD.i2c import CharLCD
import time
from datetime import datetime
import os
import random
import requests

# ==== CONFIG ====
WEATHER_API_KEY = 'AAAA'
LAT = '47.68'
LON = '-122.16'

# Define your LCD setup
lcd = CharLCD(
    i2c_expander='PCF8574',
    address=0x27,          # I2C address of your LCD (usually 0x27 or 0x3F)
    port=1,                # Typically 1 on Raspberry Pi
    cols=16,               # Number of columns
    rows=2,                # Number of rows
)

weather_text = "0"  # Global cached weather

def update_weather():
    global weather_text
    weather_text = get_weather()
    print(weather_text)

def get_weather():
    try:
        # Construct the API URL (using 2.5 version, with lat, lon, and Fahrenheit)
        url = f'http://api.openweathermap.org/data/2.5/weather?lat={LAT}&lon={LON}&appid={WEATHER_API_KEY}&units=imperial&lang=en'

        # Make the API request
        response = requests.get(url)
        data = response.json()

        # Extract current weather information
        temp = data['main']['temp']  # Temperature in Fahrenheit
        description = data['weather'][0]['description']  # Weather description (e.g., "clear sky")

        # Format the weather information
        weather_info = f"{temp:.1f}F"
        return weather_info

    except Exception as e:
        return f"Error fetching weather: {str(e)}"


try:
    lcd.clear()
    update_weather()
    while True:
        # Get current date and time
        now = datetime.now()
        date_str = now.strftime("%b %d, %Y")    # e.g., Apr 20 2025
        time_str = now.strftime("%a, %H:%M")    # e.g., 14:30

        # Write to LCD
        lcd.cursor_pos = (0, 2)
        lcd.write_string(date_str)
        lcd.cursor_pos = (1, 0)
        time_wea = time_str + " " +  weather_text
        lcd.write_string(time_wea)
#        print("Minute: ", now.minute)

        if now.minute in (0, 30):
            print("Minute: " + str(now.minute) + "; about to call update_weather()")
            update_weather()

        seconds_to_next_minute = 60 - now.second
        time.sleep(seconds_to_next_minute)

except KeyboardInterrupt:
    lcd.clear()
    print("Stopped.")
