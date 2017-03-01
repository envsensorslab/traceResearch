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
}

void loop() {

  initRTC();

}

void initRTC()
{
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //LogPrint(SYSTEM, LOG_ERROR, F("Could not find RTC"));
    while (1);
  }
  //Since this is the operation mode, only adjust the RTC time if it was complied recently
  //The assumption is made that the Setup Mode was recently run before this so the RTC vary should not 
  //Be that much
  time_t compileTime = DateTime(F(__DATE__), F(__TIME__)).unixtime();
  DateTime now = rtc.now();
  time_t nowT = now.unixtime();
  long int timeDif = nowT - compileTime;
  // Get the absolute value
  timeDif = (timeDif * timeDif)/timeDif;
  // Reset the RTC to the compile time if the difference is less than 10 seconds
  // This prevents the RTC from reseting if the arudino shuts off and gets woken back up 
  if ( timeDif < 10)
  {
    Serial.println("Adjusting");
//    LogPrint(SYSTEM, LOG_INFO, F("Adj rtc"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.print("RTC value equals: ");
  String t ="";
  timestamp(t);
  Serial.println(t);
  Serial.println(""); 
  delay(2000);
}

void timestamp(String &timeFormat)
{
    DateTime now = rtc.now();
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}


