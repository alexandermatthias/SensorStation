/*
  AirQuality Demo V1.0.
  connect to A6 to start testing. it will needs about 20s to start 
* By: http://www.seeedstudio.com

Alexander Jacobson Edits : connected Sensor A to A6 on the Mega, and I connected Sensor B to A7 on the Mega. 
I can at least get a strong positive and a strong negative signal with this.

Tested and works

*/
#include"AirQuality.h"
#include"Arduino.h"
AirQuality airqualitysensor;
int current_quality =-1;
void setup()
    {
        Serial.begin(9600);
        airqualitysensor.init(A6);
    }
void loop()
    {
    current_quality = airqualitysensor.slope();
    
    // If a valid signal is returned, convert signal to text descriptions
    if (current_quality >= 0)
        {
        if (current_quality == 0)
            Serial.println("High pollution! Force signal active");
        else if (current_quality == 1)
            Serial.println("High pollution!");
        else if (current_quality == 2)
            Serial.println("Low pollution!");
        else if (current_quality == 3)
            Serial.println("Fresh air");
        }
    }
    
    // VOC Sensor Run after loop
    ISR(TIMER2_OVF_vect)
        {
        // Set 2 seconds as a detected duty
        if(airqualitysensor.counter == 122)
            {
            airqualitysensor.last_vol = airqualitysensor.first_vol;
            airqualitysensor.first_vol = analogRead(A6);
            airqualitysensor.counter = 0;
            airqualitysensor.timer_index = 1;
            PORTB = PORTB^0x20;
            }
        else
            {
            airqualitysensor.counter++;
            }
        }