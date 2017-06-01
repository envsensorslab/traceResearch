//the purpose of this code is to get, print, and port temperature data to the SD Card and add timestamps
//sub-function: playground for testing EEPROM data


#include <Wire.h>       
#include <avr/sleep.h>


const int temperaturePin = 0;  //defines data type for analog input
#include <EEPROM.h>            //EEPROM library
#include <SD.h>
#include <SPI.h>
const int chipSelect = 10;
int counter = 0;

File tempLog;



//int address = 0;
//int EEsize = 1024;             // size in bytes of UNO's EEPROM
//byte value;

void setup() {

   Wire.begin();
   Serial.begin(9600);
  
 
 Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
   
    Serial.println("initialization failed!");
    return;
  }

  
  Serial.println("initialization done.");
  
 
}
 

void loop() {

  File logFile = SD.open("tempLog.txt", FILE_WRITE);

  //SD card
  if(logFile){
    String t = "";
    String myStr = "test String";
    logFile.println(myStr);
    Serial.println(myStr);    
    logFile.close();
    
  }
    else {Serial.println("error opening file");
  }
  delay(3000);
  sleepNow();
}

void sleepNow()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
    Serial.println("Going to sleep");     
    delay(1000);

 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
                        
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep... 
    

    Serial.println("Woke up!");
                             
} 

