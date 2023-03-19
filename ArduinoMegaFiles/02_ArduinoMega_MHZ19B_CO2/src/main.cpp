// Necessary to use arduino
#include <Arduino.h>
// All necessary libraries should be in lib folder
#include <MHZ19_uart.h>

// Define the output pins
// Connect the Sensor Rx (orange cable) to Arduino Mega pin 50
// Connect the Sensor Tx (blue cable) to Arduino Mega pin 51
//WORKS
//const int tx_pin = 50;	
//const int rx_pin = 51;
//DOESNT work
const int CO2tx_pin = 50;	
const int CO2rx_pin = 51;


MHZ19_uart mhz19;

// put your setup code here, to run once:
void setup() {
  Serial.begin(9600);
  //Serial.begin(9600);

  // Initialize CO2 reader
  mhz19.begin(CO2rx_pin, CO2tx_pin);
  mhz19.setAutoCalibration(false);

}


// put your main code here, to run repeatedly:
void loop() {

  // Read CO2 and temperature data
  int co2ppm = mhz19.getPPM();
  int temp = mhz19.getTemperature();

  // Print CO2 and temperature data
  Serial.print("co2: "); Serial.println(co2ppm);
  Serial.print("temp: "); Serial.println(temp);
  
  delay(1000);

}