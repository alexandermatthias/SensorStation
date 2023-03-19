// This sketch sets the date and time on RTC
// To use this sketch you need the DS3231 RTC Clock module
/*
Pin Setup
DS3231:
DS3231-GND goes to GND on Ardiuno
DS3231-VCC goes to 5V on Ardiuno
DS3231-SDA goes to A4 on Ardiuno
DS3231-SCL goes to A5 on Ardiuno


HW-125 Card Module:
HW-125-CS goes to pin 10 on Arduino
HW-125-SCK goes to pin 13 on Arduino
HW-125-MOSI goes to pin 12 on Arduino
HW-125-MISO goes to pin 11 on Arduino
HW-125-VCC goes to 5V on Arduino
HW-125-GND goes to GND 10 on Arduino
*/ 

#include <Arduino.h>
#include <DS3231.h>
#include <Wire.h>

// Get the function names from here: C:\Users\emack\Documents\PlatformIO\Projects\DS3231_2\.pio\libdeps\uno\RTC\src\DS3231.cpp
// Set the time with this sketch: C:\Users\emack\Documents\PlatformIO\Projects\SET_DS3231_time\src\main.cpp

DS3231 Clock;

/*
bool h12;
bool PM;
*/

void setup() {

Wire.begin();

// Clock.setClockMode(h12);
Clock.setSecond(0);//Set the second
Clock.setMinute(52);//Set the minute
Clock.setHour(7);  //Set the hour
Clock.setDoW(1);    //Set the day of the week
Clock.setDate(24);  //Set the date of the month
Clock.setMonth(1);  //Set the month of the year
Clock.setYear(21);  //Set the year (Last two digits of the year)

Serial.begin(9600);

}

void loop()
{

}