// The purpose of this function is to quickly compile and set the RTC time

#include "RTClib.h"
#include <time.h>


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
}

void initRTC()
{
  if (! rtc.begin()) {
    while (1);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

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


