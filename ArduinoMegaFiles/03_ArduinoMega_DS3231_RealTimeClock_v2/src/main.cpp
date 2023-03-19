/*
* Arduino DS3231 Real Time Clock Module Tutorial
*
* Crated by Dejan Nedelkovski,
* www.HowToMechatronics.com
*
* DS3231 Library made by Henning Karlsen which can be found and downloaded from his website, www.rinkydinkelectronics.com.
*
*/
#include <DS3231.h>

// DS3231  rtc(SDA, SCL);
DS3231  rtc(A4, A5);

void setup() { 
 Serial.begin(9600);
 rtc.begin(); // Initialize the rtc object
 rtc.setTime(16,20,40); //Set the time to 16:20:40
 rtc.setDate(18,05,2022);
}

void loop() { 
 Serial.println();
 Serial.print("Time:  ");
 Serial.print(rtc.getTimeStr());
 Serial.println();

 Serial.print("Date: ");
 Serial.print(rtc.getDateStr());
 Serial.println();
 
 delay(1000); 
}