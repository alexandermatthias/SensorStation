// WEATHER STATION - Instructions
// 1. Add code for each sensor under it's own '-----------' separator
// 2. Each new sensors code should appear in the sequence that they are read in LOOP

// ----------------------------------------------
// REAL TIME CLOCK
// Hardware hookup:
// Connect GND to GND
// Connect VCC to 3.3V POWER
// Connect SDA to A4
// Connect SCI to A5
#include <DS3231.h>
// DS3231  rtc(SDA, SCL);
DS3231  rtc(A4, A5);

// ----------------------------------------------
// TEMPERATURE AND RELATIVE HUMIDITY
// When looking at the sensor with grates facing you an pointed up
 
// Hardware hookup:
// Count pins from left to right
// Connect sensor pin 1 to Arduino 5V power
// Connect sensor pin 2 ArduinoMega pin 22
// Leave sensor pin 3 disconnected
// Connect sensor pin 4 to Arduino GND

#include <Arduino.h>
#include <DHT.h>
#define DHTPIN 22     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
int maxHum = 60;
int maxTemp = 40;
DHT dht(DHTPIN, DHTTYPE);


// ----------------------------------------------
// WIND SPEED AND TEMPERATURE
// Requires a Wind Sensor Rev P from Modern Device
// Code is based on Paul Badger's publid domain code from 2014
// http://moderndevice.com/product/wind-sensor-rev-p/
// The Rev P requires at least at least an 8 volt supply. The easiest way to power it 
// if you are using an Arduino is to use a 9 volt or higher supply on the external power jack
// and power the sensor from Vin.
// 

// Hardware hookup:
// Sensor   to   Arduino Pin
// Ground   to   Ground
// +10-12V  to     Vin
// Out      to     A0
// TMP      to     A1
//
// #include <Arduino.h>
const int WindOutPin  = A0;   // wind sensor analog pin  hooked up to Wind P sensor "OUT" pin
const int WindTempPin = A1;   // temp sesnsor analog pin hooked up to Wind P sensor "TMP" pin
/*
*/

// ----------------------------------------------
// FAKE DATA
/*
int fakeRecordedData = 0;
*/

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
  // SETUP - REAL TIME CLOCK
  rtc.begin(); // Initialize the rtc object
  // The following two lines are used to set the time
  // rtc.setTime(13,59,00); //Set the time to 16:20:40
  // rtc.setDate(02,8,2022);
  Serial.println();
  Serial.println("Real Time Clock initialization done.");

  // --------------------------------------------------------------------------------------------
  // SETUP - TEMPERATURE AND RELATIVE HUMIDITY SETUP
  /*
  dht.begin();
  Serial.println("Temperature and Relative Humidity initialization done.");
  */

  // --------------------------------------------------------------------------------------------
  // SETUP - SD CARD
  // Construct a set of comma separated headers for recording data
  String separator = String(", ");
  String dataHeaders = String("Time (hh:mm:ss)");
  dataHeaders += separator;
  dataHeaders += String("Temperature (*C)");
  dataHeaders += separator;
  dataHeaders += String("Humidity (%)");
  dataHeaders += separator;
  dataHeaders += String("Wind Speed (Mps)");
  dataHeaders += separator;
  dataHeaders += String("Wind Temperature (*C)");
  
  
  Serial.print("Data headers: ");
  Serial.println(dataHeaders);
  Serial.print("Initializing SD card...");
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(53, OUTPUT);
 
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
 // LOOP - REAL TIME CLOCK
 Serial.println();
 Serial.print("Time:  ");
 Serial.print(rtc.getTimeStr());
 Serial.println();
 Serial.print("Date: ");
 Serial.print(rtc.getDateStr());
 Serial.println();
 
 // --------------------------------------------------------------------------------------------
 // LOOP - TEMPERATURE AND RELATIVE HUMIDITY
 // Reading temperature or humidity takes about 250 milliseconds!
 // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 
 float humidity = dht.readHumidity();
 // Read temperature as Celsius
 float temperature = dht.readTemperature();
 
 // Check if any reads failed and exit early (to try again).
 if (isnan(humidity) || isnan(temperature)) {
   Serial.println("Failed to read from DHT sensor!");
   return;
 }
 Serial.print("Humidity: "); 
 Serial.print(humidity);
 Serial.print(" %\t");
 Serial.print("Temperature: "); 
 Serial.print(temperature);
 Serial.println(" *C ");
 
 
 // --------------------------------------------------------------------------------------------
 // LOOP - WIND SPEED AND TEMPERATURE
 // read wind
 int windADunits = analogRead(WindOutPin);
 // Serial.print("RW ");   // print raw A/D for debug
 // Serial.print(windADunits);
 // Serial.print("\t");

 // wind formula derived from a wind tunnel data, annemometer and some fancy Excel regressions
 // this scalin doesn't have any temperature correction in it yet
 float windMPH =  pow((((float)windADunits - 264.0) / 85.6814), 3.36814);
 float windMpS = windMPH * 0.44704;
 Serial.print("Wind speed read:  ");
 Serial.print(windMpS);
 Serial.print(" MpS\t");    

 // temp routine and print raw and temp C
 int tempRawAD = analogRead(WindTempPin);  
 // Serial.print("RT ");    // print raw A/D for debug
 // Serial.print(tempRawAD);
 // Serial.print("\t");

 // convert to volts then use formula from datatsheet 
 // Vout = ( TempC * .0195 ) + .400
 // tempC = (Vout - V0c) / TC   see the MCP9701 datasheet for V0c and TC

 float windTempC = ((((float)tempRawAD * 5.0) / 1024.0) - 0.400) / .0195; 
 Serial.print(windTempC);
 Serial.println(" C");


 // --------------------------------------------------------------------------------------------
 // LOOP - DELAY BETWEEN SENSOR READ START AND SENSOR DATA WRITE
 delay(1000); 

 // --------------------------------------------------------------------------------------------
 // LOOP - CREATE STRING TO PRINT
 // Check to make sure that each sensors input is in the same
 // order as in the dataHeaders string above
 String separator = String(", ");
 String stringToWrite = rtc.getTimeStr();
 stringToWrite += separator;
 stringToWrite += String(temperature);
 stringToWrite += separator;
 stringToWrite += String(humidity);
 stringToWrite += separator;
 stringToWrite += String(windMpS);
 stringToWrite += separator;
 stringToWrite += String(windTempC);
 
 
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
 // fakeRecordedData += 1; 
 delay(3000); 
}