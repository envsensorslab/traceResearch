#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>
#include <DS1307RTC.h> 


RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {

Wire.begin();
Serial.begin(9600);

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  
    Serial.println("RTC lost power, resetting time");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(DateTime(F(__DATE__), F(__TIME__)));
  

}

void loop() {
  // put your main code here, to run repeatedly:
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
    delay(3000);
}




//sub-functions of this sub-function 

//this function converts decimal numbers to binary coded decimals 
byte decToBcd (byte val){
  return((val/10*16)+(val%10));
}

//this function does the reverse- binary to normal decimals
byte bcdToDec(byte val){
  return((val/16*10)+(val%16));
}


