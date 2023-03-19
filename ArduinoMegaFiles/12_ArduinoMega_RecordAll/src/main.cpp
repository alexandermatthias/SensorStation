// WEATHER STATION - Instructions
// 1. Add code for each sensor under it's own '-----------' separator
// 2. Each new sensors code should appear in the sequence that they are read in LOOP

/*
NOTES ON THIS VERSION OF THE CODE
- Currently the clock is being reset each time setup runs. You should comment out that line of the code 
- If the VOC sensor does not read, it locks up the entire read sequence. 
    This should be change to a failure reporting instead
- There are some connectivity issues on the hardware of the VOC sensor. We'll need to address this.


*/

// -----------------------------------------------------------------------------------------------
// REAL TIME CLOCK
// Connections notes:
// Connect GND to GND
// Connect VCC to 3.3V POWER
// Connect SDA to A4
// Connect SCI to A5
#include <DS3231.h>
// DS3231  rtc(SDA, SCL);
DS3231  rtc(A4, A5);


// -----------------------------------------------------------------------------------------------
// TEMPERATURE AND RELATIVE HUMIDITY
// When looking at the sensor with grates facing you an pointed up
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

/*
// -----------------------------------------------------------------------------------------------
// CO2 SENSOR
// Arduino.h is necessary to use arduino (Already included above)
// #include <Arduino.h>
// All necessary libraries should be in lib folder
#include <MHZ19_uart.h>
// Connect the Sensor Rx (orange cable) to Arduino Mega pin 50
// Connect the Sensor Tx (blue cable) to Arduino Mega pin 51
// Connect sensor GND to Arduino GND
// Connect sensor Vin to Arduino 5V power
//WORKS
const int CO2tx_pin = 50;	
const int CO2rx_pin = 51;

//const int CO2rx_pin = 18;
//const int CO2tx_pin = 19;	
MHZ19_uart mhz19;
*/

// -----------------------------------------------------------------------------------------------
// PM 2.5 - PM 10 sensor
// Connect Sensor Pin 5 GND Ground (red cable) to Arduino GND
// Connect Sensor Pin 4 SEL Interface Select (black cable) to Arduino GND (this selects I2C)
// Connect Sensor Pin 3 TX or I2C Serial Clock Input SCL (yellow cable) to Arduino 21 SCL
// Connect Sensor Pin 2 RX or I2C Serial Data Input SDA (green cable) to Arduino 20 SDA
// Connect Sensor Pin 1 VDD Supply Voltage 5V (blue cable) to Arduino 5V power supply
#include <sps30.h>

// Arduino.h is necessary to use arduino (Already included above)
// #include"Arduino.h"

// Example arduino sketch, based on 
// https://github.com/Sensirion/embedded-sps/blob/master/sps30-i2c/sps30_example_usage.c

// Uncomment the next line to use the serial plotter (This is commented out by default)
// #define PLOTTER_FORMAT

// -----------------------------------------------------------------------------------------------
// VOC Sensor 
// AirQuality Demo V1.0.
// connect to A6 to start testing. it will needs about 20s to start 
// By: http://www.seeedstudio.com
// Connect Sensor GND Ground (blue cable) to Arduino GND
// Connect Sensor 5V (purple cable) to Arduino 5V power supply
// Connect Sensor A (grey cable) to Arduino A6
// Connect Sensor B (white cable) to Arduino A7

#include"AirQuality.h"
// (Already included Arduino.h above)
//#include"Arduino.h"

AirQuality airqualitysensor;
int current_quality =-1;

// -----------------------------------------------------------------------------------------------
// FAKE DATA
int fakeRecordedData = 0;

// -----------------------------------------------------------------------------------------------
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
    Serial.println("Real Time Clock initialization...");
    rtc.begin(); // Initialize the rtc object
    // The following two lines are used to set the time
    
    // Uncomment the following block to reset the clock at the date and
    // time specified below. Note, you have to push the Arduino reset button about 2 seconds
    // before the time for the time recorded on the SD card to appear exactly the same because
    // there are some delay() commands in the code
    /*
    Serial.println("Clock time has been reset to value specified in code.");
    // Set the time to HH, MM, SS
     rtc.setTime(22,46,00); 
    // set the date to DD, MM, YYYY
     rtc.setDate(16,10,2022);
    */
    
    Serial.println("Real Time Clock initialization done.");
    Serial.println();

    // --------------------------------------------------------------------------------------------
    // SETUP - TEMPERATURE AND RELATIVE HUMIDITY SETUP
    dht.begin();
    Serial.println("Temperature and Relative Humidity initialization done.");
    Serial.println();

    /*
    // --------------------------------------------------------------------------------------------
    // SETUP - CO2 SENSOR
    // Initialize CO2 reader
    mhz19.begin(CO2rx_pin, CO2tx_pin);
    mhz19.setAutoCalibration(false);
    Serial.println();
    */

    // --------------------------------------------------------------------------------------------
    // SETUP - PM 2.5 - PM 10 sensor
    // SENSIRION SPS30 SETUP
    Serial.println("Initializing Sensirion Particulate Matter Sensor...");
    int16_t ret;
    uint8_t auto_clean_days = 4;
    uint32_t auto_clean;
    // Serial.begin is noted above
    // Serial.begin(9600);
    
    // Why is this delay necessary? 
    delay(2000);
    sensirion_i2c_init();
    // STEP 1 - Test if sensor probing failed
    // Note, that if this sensor fails, this will stop all readings. 
    // Not a good protocol.
    while (sps30_probe() != 0) 
        {
        Serial.println("SPS sensor probing failed");
        delay(500);
        }

    // STEP 2 - If the user has uncommented PLOTTER_FORMAT above, then print this
    // through the serial plotter
    #ifndef PLOTTER_FORMAT
        Serial.println("SPS sensor probing successful");
    #endif /* PLOTTER_FORMAT */

    // STEP 3 - Attempt to set the Auto-clean interval
    ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
    if (ret) 
        {
        Serial.print("error setting the auto-clean interval: ");
        Serial.println(ret);
        }

    // STEP 4 - Start measuring
    ret = sps30_start_measurement();
    // Case 1 - Error starting the measurement
    if (ret < 0)
        {
        Serial.println("error starting measurement");
        }

    // If the user has uncommented PLOTTER_FORMAT above, then say we have started measuring.
    #ifndef PLOTTER_FORMAT
        Serial.println("measurements started");
    #endif /* PLOTTER_FORMAT */

    // If this variable exists, then there is a problem. I assume this variable is created
    // in the sps30 library.
    #ifdef SPS30_LIMITED_I2C_BUFFER_SIZE
        Serial.print("Your Arduino hardware has a limitation that only\n");
        Serial.print("  allows reading the mass concentrations. For more\n");
        Serial.print("  information, please check\n");
        Serial.print("  https://github.com/Sensirion/arduino-sps#esp8266-partial-legacy-support\n");
        Serial.print("\n");
        delay(2000);
    #endif
    Serial.println("Sensirion Particulate Matter initialization done.");
    Serial.println();

    // Wait 1 second before proceeding to LOOP (presumably to give it time to measure)
    delay(1000);

    /*
    // -----------------------------------------------------------------------------------------------
    // SETUP - VOC Sensor 
    // (Serial is already started above)
    // Serial.begin(9600);
    Serial.println("Initializing VOC sensor...");
    airqualitysensor.init(A6);
    Serial.println("VOC sensor initializing done.");
    Serial.println();
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
    // dataHeaders += String("CO2 (PPM)");
    // dataHeaders += separator;
    dataHeaders += String("PM 1.0");
    dataHeaders += separator;
    dataHeaders += String("PM 2.5");
    dataHeaders += separator;
    dataHeaders += String("PM 4.0");
    dataHeaders += separator;
    dataHeaders += String("PM 10.0");
    dataHeaders += separator;
    dataHeaders += String("VOC_AirQuality");


    Serial.print("Data headers: ");
    Serial.println(dataHeaders);
    Serial.print("Initializing SD card...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
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

    /*
    // --------------------------------------------------------------------------------------------
    // LOOP - CO2 SENSOR
    // Read CO2 and temperature data
    int co2ppm = mhz19.getPPM();
    int temp = mhz19.getTemperature();

    // Print CO2 and temperature data
    Serial.print("co2: "); Serial.println(co2ppm);
    Serial.print("temp: "); Serial.println(temp);
    */

    // --------------------------------------------------------------------------------------------
    // LOOP - PM 2.5 - PM 10 SENSOR
    // SENSIRION SPS30 SETUP
    struct sps30_measurement m;
    char serial[SPS30_MAX_SERIAL_LEN];
    uint16_t data_ready;
    int16_t ret;


    // Define plotting variables
    float PPM_1p0 = m.mc_1p0;
    float PPM_2p5 = m.mc_2p5;
    float PPM_4p0 = m.mc_4p0;
    float PPM_10p0 = m.mc_10p0;

    // STEP 1 - CHECK IF THERE IS NEW DATA READY
    do
    {
        ret = sps30_read_data_ready(&data_ready);
        // CASE 1 - failed to read the flag for new data being ready
        if (ret < 0)
            {
            Serial.print("error reading data-ready flag: ");
            Serial.println(ret);
            }
        // CASE 2 - read the flag, but the data isn't ready yet
        else if (!data_ready)
            Serial.print("data not ready, no new measurement available\n");
        // CASE 3 - flag was read, and new data is ready
        else
            break;
        
        // Wait 0.1 seconds
        delay(100); /* retry in 100ms */
    } 
    // This is an endless loop, broken only when the flag is 
    // successfully read and the new data is ready.
    while (1);

    // STEP 2 - READ THE DATA
    ret = sps30_read_measurement(&m);

    // CASE 1 - Unsuccessful measurement
    if (ret < 0)
    {
        Serial.print("error reading measurement\n");
    }

    // CASE 2 - Successful measurement
    else
    {
      
        // Case 2 STEP 1 - PLOTTER_FORMAT 
        // If PLOTTER_FORMAT has not been defined, then then compiler should 
        // include everything (up to the PLOTTER_FORMAT code below)
        #ifndef PLOTTER_FORMAT
            Serial.print("PM  1.0: ");
            Serial.println(m.mc_1p0);
            Serial.print("PM  2.5: ");
            Serial.println(m.mc_2p5);
            Serial.print("PM  4.0: ");
            Serial.println(m.mc_4p0);
            Serial.print("PM 10.0: ");
            Serial.println(m.mc_10p0);

        // CASE 2A - Buffer too big
        // If SPS30_LIMITED_I2C_BUFFER_SIZE has been defined, (from sps30 library?)
        // then compiler should everything below 
        // (up to the STOP SPS30_LIMITED_BUFFER_SIZE line below.)
        #ifndef SPS30_LIMITED_I2C_BUFFER_SIZE
            Serial.print("NC  0.5: ");
            Serial.println(m.nc_0p5);
            Serial.print("NC  1.0: ");
            Serial.println(m.nc_1p0);
            Serial.print("NC  2.5: ");
            Serial.println(m.nc_2p5);
            Serial.print("NC  4.0: ");
            Serial.println(m.nc_4p0);
            Serial.print("NC 10.0: ");
            Serial.println(m.nc_10p0);
            // Why would you only print 'Typical Particle Size' if the buffer is 
            // of limited size?
            Serial.print("Typical partical size: ");
            Serial.println(m.typical_particle_size);
        
        // STOP SPS30_LIMITED_I2C_BUFFER_SIZE code block
        #endif
            Serial.println();

        // CASE 2B - Buffer is fine
        // If SPS30_LIMITED_I2C_BUFFER_SIZE is evaluated as false, (i.e. no problems)
        // then compiler should include this code
        #else
            // since all values include particles smaller than X, if we want to create buckets we 
            // need to subtract the smaller particle count. 
            // This will create buckets (all values in micro meters):
            // - particles        <= 0,5
            // - particles > 0.5, <= 1
            // - particles > 1,   <= 2.5
            // - particles > 2.5, <= 4
            // - particles > 4,   <= 10

            Serial.print(m.nc_0p5);
            Serial.print(" ");
            Serial.print(m.nc_1p0  - m.nc_0p5);
            Serial.print(" ");
            Serial.print(m.nc_2p5  - m.nc_1p0);
            Serial.print(" ");
            Serial.print(m.nc_4p0  - m.nc_2p5);
            Serial.print(" ");
            Serial.print(m.nc_10p0 - m.nc_4p0);
            Serial.println();



        // STOP PLOTTER_FORMAT code block
        #endif
    }

    // Wait 1 second before looping again
    delay(1000);

    
    // --------------------------------------------------------------------------------------------
    // LOOP - VOC SESNOR
    String VOC_description = "No data";
    /*
    Serial.println("VOC sensor reading...");

    current_quality = airqualitysensor.slope();
    // If a valid signal is returned, convert signal to text descriptions
    if (current_quality >= 0)
        {
        if (current_quality == 0)
            {
            VOC_description = "High pollution! Force signal active";
            Serial.println("High pollution! Force signal active");
            }
        else if (current_quality == 1)
            {
            VOC_description = "High pollution";
            Serial.println("High pollution");
            }
        else if (current_quality == 2)
            {
            VOC_description = "Low pollution";
            Serial.println("Low pollution");
            }
        else if (current_quality == 3)
            {
            VOC_description = "Fresh air";
            Serial.println("Fresh air");
            }
        }
    */

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
    // stringToWrite += String(co2ppm);
    // stringToWrite += separator;
    stringToWrite += String(PPM_1p0);
    stringToWrite += separator;
    stringToWrite += String(PPM_2p5);
    stringToWrite += separator;
    stringToWrite += String(PPM_4p0);
    stringToWrite += separator;
    stringToWrite += String(PPM_10p0);
    stringToWrite += separator;
    stringToWrite += String(VOC_description);

           
            

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
    delay(600000); 
}
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