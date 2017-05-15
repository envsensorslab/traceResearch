#include <Wire.h>
#include "RTClib.h"
#include <avr/sleep.h>

RTC_DS3231 rtc;

float temp;

#define interruptPin 2

void setup() {
  Serial.begin(9600);
  rtc.begin();
  Serial.println("RTC Started");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(interruptPin, INPUT);
  //pinMode(interruptPin, INPUT_PULLUP);
}

void loop() {

    DateTime now = rtc.now();
    String t="";
    timestamp(t);
    Serial.print("Time: ");
    Serial.println(t);
    //temp = rtc.requestTemp();
    //Serial.print("Temperature in C: ");
    //Serial.println(temp);
    
    
    rtc.enableAlarm1();    
    DateTime setAlarm = now + TimeSpan(0,0,1,0);
    Serial.print("Set alarm: ");
    printDate(setAlarm);
    rtc.setAlarm1(setAlarm);

    sleepNow();
    rtc.disableAlarm1();
    
    Serial.println("pause");
    delay(3000);


}

void timestamp(String &timeFormat){
    DateTime now = rtc.now();
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}

void printDate(DateTime &timeDate){
    DateTime now = timeDate;
    String timeFormat = "";
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
    Serial.println(timeFormat);
  
}

void indicate(){
  // execute code here after wake-up before returning to the loop() function  
  // timers and code using timers (serial.print and more...) will not work here.  
  // we don't really need to execute any special functions here, since we  
  // just want the thing to wake up  
  Serial.println("Indicate");
}

void sleepNow()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
    Serial.println("Going to sleep");     
    delay(100);
    attachInterrupt(digitalPinToInterrupt(interruptPin), indicate , LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
                        
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep... 
    
    detachInterrupt(digitalPinToInterrupt(interruptPin));      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
    Serial.println("Woke up!");
                             
} 



