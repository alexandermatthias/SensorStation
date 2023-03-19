// SD Card Reader
#include <SD.h>
File myFile;

// Real Time Clock
#include <DS3231.h>
// DS3231  rtc(SDA, SCL);
DS3231  rtc(A4, A5);

// FAKE DATA
int fakeRecordedData = 0;

 
void setup()
{
  Serial.begin(9600);
  
  // REAL TIME CLOCK SETUP
  rtc.begin(); // Initialize the rtc object
  rtc.setTime(16,20,40); //Set the time to 16:20:40
  rtc.setDate(18,05,2022);


  
  // SD CARD SETUP
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(53, OUTPUT);
 
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 5, 6, 7.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
 
void loop()
{

 // REAL TIME CLOCK PRINT TIME
 Serial.println();
 Serial.print("Time:  ");
 Serial.print(rtc.getTimeStr());
 Serial.println();
 Serial.print("Date: ");
 Serial.print(rtc.getDateStr());
 Serial.println();
 
 // CREATE STRING TO PRINT
 String stringToWrite = rtc.getTimeStr() + String(", ") + String(fakeRecordedData) ;
 Serial.print("Data written:  ");
 Serial.print(stringToWrite);
 Serial.println();

 // SD CARD WRITE 
 myFile = SD.open("test.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
        myFile.println(stringToWrite);
	// close the file:
    myFile.close();
    Serial.println("Wrote time to SD Card.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // FAKE RECORDED DATA
  fakeRecordedData += 1; 
 delay(3000); 
}