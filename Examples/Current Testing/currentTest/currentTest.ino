//the purpose of this code is to get, print, and port temperature data to the SD Card and add timestamps
//sub-function: playground for testing EEPROM data

    
//#include <avr/sleep.h>
#include "LowPower.h"
#include <RTClib.h>
#include <SD_modified.h>
#include <Adafruit_ADS1015.h>

RTC_DS3231 rtc;
Adafruit_ADS1115 ads1115;

#define interruptPin 2
#define sensorEnable A0


char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(interruptPin, INPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(sensorEnable, OUTPUT);
  digitalWrite(sensorEnable, HIGH);
  initSDCard();
  
  Serial.println("Starting RTC");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  
    Serial.println("RTC lost power, resetting time");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("Starting main");
    
    ads1115.begin();
    

    ads1115.startComparator_windowed(1, 7000, 700);



}

void indicate(){
  // execute code here after wake-up before returning to the loop() function  
  // timers and code using timers (serial.print and more...) will not work here.  
  // we don't really need to execute any special functions here, since we  
  // just want the thing to wake up 
}

void loop() {
    
    

    writeDate();
    
    int32_t adc1;
  ads1115.startComparator_windowed(1, 20000, 700);
  adc1 = ads1115.getLastConversionResults();
  Serial.print("AIN1: "); Serial.println(adc1);
  sleepNow();
  ads1115.disableComparatorAlert();
}

void sleepNow()
{
    Serial.println("Going to sleep");  
    digitalWrite(sensorEnable, LOW);   
    delay(100);
    // Clear the comparator right before attaching interrupt   
    ads1115.getLastConversionResults();
    attachInterrupt(digitalPinToInterrupt(interruptPin), indicate , LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
   
    detachInterrupt(digitalPinToInterrupt(interruptPin));      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
    digitalWrite(sensorEnable, HIGH);
    initSDCard();
    writeLogFile();
    Serial.println("Woke up!");
                             
} 

void initSDCard()
{  
  Serial.print("Initializing SD card...");  
  if (!SD.begin(10)) 
  {   
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
}

void writeLogFile()
{
 // put your main code here, to run repeatedly:
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
}

void writeDate()
{
  DateTime now = rtc.now();
    Serial.println("Current Arudino time is");
    Serial.println("");
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    Serial.println();
}
 

