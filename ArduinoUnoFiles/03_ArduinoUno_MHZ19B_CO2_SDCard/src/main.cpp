// WEATHER STATION - Instructions
// 1. Add code for each sensor under it's own '-----------' separator
// 2. Each new sensors code should appear in the sequence that they are read in LOOP

// THIS SKETCH WORKS, BUT ONLY IF THERE IS An Extra 5V power supply available
// from the MEGA because both the SD card and the CO2 require 5V

// ----------------------------------------------
// CO2 SENSOR
// Arduino.h is necessary to use arduino (Already included above)
// #include <Arduino.h>
// All necessary libraries should be in lib folder
#include <MHZ19_uart.h>
// Connect the Sensor Rx (orange cable) to Arduino pin 5
// Connect the Sensor Tx (blue cable) to Arduino pin 4
// Connect sensor GND to Arduino GND
// Connect sensor Vin to Arduino 5V power
//WORKS
const int CO2tx_pin = 4;	
const int CO2rx_pin = 5;

//const int CO2rx_pin = 18;
//const int CO2tx_pin = 19;	
MHZ19_uart mhz19;

// ----------------------------------------------
// FAKE DATA
int fakeRecordedData = 0;

// ----------------------------------------------
// SD CARD 
// Connections notes:
// Connect the 5V pin to the 5V pin
// Connect the GND pin to the GND pin 
// Connect CLK to pin 52
// Connect DO to pin 50
// Connect DI to pin 51
// Connect CS to pin 53
#include <SD.h>
File myFile;


 
void setup()
{
  Serial.begin(9600);
  

  
  // --------------------------------------------------------------------------------------------
  // SETUP - CO2 SENSOR
  // Initialize CO2 reader
  mhz19.begin(CO2rx_pin, CO2tx_pin);
  mhz19.setAutoCalibration(false);

  // --------------------------------------------------------------------------------------------
  // SETUP - SD CARD
  // Construct a set of comma separated headers for recording data
  String separator = String(", ");
  String dataHeaders = String("FakeData (Count)");
  dataHeaders += separator;
  dataHeaders += String("CO2 (PPM)");
  
  
  Serial.print("Data headers: ");
  Serial.println(dataHeaders);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
 
  // Attempt to initilize SD library
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("SD Card initialization done.");
 
  // Open the file. Note that only one file can be open at a time,
  // It is necessary to close this file before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
 
  // If the file opened okay, write to it:
  if (myFile) {
    Serial.println("Successfully wrote headers to log file on SD Card.");
    myFile.println(dataHeaders);
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Failed to open the log file on the SD Card");
  }
}
 
void loop()
{
 
 // --------------------------------------------------------------------------------------------
 // LOOP - CO2 SENSOR
 // Read CO2 and temperature data
  int co2ppm = mhz19.getPPM();
  int temp = mhz19.getTemperature();

  // Print CO2 and temperature data
  Serial.print("co2: "); Serial.println(co2ppm);
  Serial.print("temp: "); Serial.println(temp);


 // --------------------------------------------------------------------------------------------
 // LOOP - DELAY BETWEEN SENSOR READ START AND SENSOR DATA WRITE
 delay(1000); 

 // --------------------------------------------------------------------------------------------
 // LOOP - CREATE STRING TO PRINT
 // Check to make sure that each sensors input is in the same
 // order as in the dataHeaders string above
 String separator = String(", ");
 String stringToWrite = String(fakeRecordedData);
 stringToWrite += separator;
 stringToWrite += String(co2ppm);
 
 Serial.print("Data written:  ");
 Serial.print(stringToWrite);
 Serial.println();

 // --------------------------------------------------------------------------------------------
 // LOOP - SD CARD WRITE 
 myFile = SD.open("test.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
        myFile.println(stringToWrite);
	// close the file:
    myFile.close();
    Serial.println("Wrote data to SD Card.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Error opening test.txt on SD Card");
  }
 
 // --------------------------------------------------------------------------------------------
 // LOOP - DELAY UNTIL NEXT RECORDING
 fakeRecordedData += 1; 
 delay(2000); 
}