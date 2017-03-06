// The purpose of this function is to quickly compile and set the RTC time

#include "RTClib.h"
#include <time.h>

#define timeValSize 3

time_t timeVal[timeValSize] = {1488469844, 1488469944, 1488470844};

enum module {
  SYSTEM
};
enum log_level {
  LOG_ERROR
};

RTC_DS3231 rtc;


void setup() {
  Serial.begin(9600);

  initRTC();
}

void loop() {
  
  String t ="";
  timestamp(t);
  Serial.println(t);

  for (int i=0; i<timeValSize; i++){
    timetest(timeVal[i]);
  }

  while (1);
}

void initRTC()
{
  if (! rtc.begin()) {
    while (1);
  }

 
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0,0,0,12));
  //Serial.print(DateTime(F(__DATE__), F(__TIME__)).unixtime());
  Serial.print("RTC value equals: ");
  String t ="";
  timestamp(t);
  Serial.println(t);
  
}

void timestamp(String &timeFormat)
{
    DateTime now = rtc.now();
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}

void timetest(time_t val){

  Serial.println("");
  Serial.println("");
  Serial.println("");
  
  DateTime now = rtc.now();
  Serial.print("Inputted time: ");
  Serial.println(val);
  String f ="";
  timeStampCustom(f,val);
  Serial.print("Inputted time stamp: ");
  Serial.println(f);
  

  time_t difference;

  if (now.unixtime() > val){
   difference = now.unixtime() - val;
   Serial.println("The desired time is in the past");
  }
  else {
    difference = val-now.unixtime();
    Serial.println("The desired time is in the future");
  }
  Serial.print("Approx difference in seconds: ");
  Serial.println(difference);
  Serial.print("Approx difference in minutes: ");
  Serial.println(difference/60);
  Serial.print("Approx difference in hours: ");
  Serial.println(difference/3600);
  
  
}

void timeStampCustom(String &timeFormat, time_t customTime)
{
    DateTime now = customTime;
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}

