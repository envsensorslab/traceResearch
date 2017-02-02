//the purpose of this code is to get, print, and port temperature data to the SD Card and add timestamps
//sub-function: playground for testing EEPROM data


#include <Wire.h>              
#include <time.h>
#define DS3231_I2C_ADDRESS 0x68

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
  
   /*Wire.beginTransmission(0x68); // address DS3231
   Wire.write(0x0E); // select register
   Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
   Wire.endTransmission();*/

   DateTime now = RTC.now();
   DateTime c
 
 //Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
   
    Serial.println("initialization failed!");
    return;
  }

  
  //Serial.println("initialization done.");
  
 
}
 

void loop() {

//temperature sensor
float voltage, degreesC;

voltage = getVoltage(temperaturePin);

degreesC = (voltage-0.5)*100.0;

File tempLog = SD.open("tempLog.txt", FILE_WRITE);

//time
// send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.write((byte)0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 1); // request three bytes (seconds, minutes, hours) (now 1 for ms)

 while(Wire.available())
  { 
    
    int seconds = Wire.read();
    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
    milliseconds = seconds*1000;


//SD card
if(tempLog){
  if (counter <= 20){

  
  Serial.print(seconds);
  Serial.print(",");
  tempLog.print(seconds);
  tempLog.print(",");
  Serial.print(degreesC);
  Serial.println("");
  tempLog.println(degreesC);
  tempLog.close();
  //Serial.println("done");

  counter = counter + 1;
}
}
else {Serial.println("error opening file");
}

delay(1000);

//int val = analogRead(0)/4;

}
}

float getVoltage(int pin)
{
  return(analogRead(pin)*0.004882814);
  
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

