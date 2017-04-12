#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1015 ads1015;

const byte volatile interruptPin = 2;

byte volatile toggle=0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("Single-ended readings from AIN0 with >3.0V comparator");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  Serial.println("Comparator Threshold: 1000 (3.000V)");
  ads1015.begin();
  
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), indicate , RISING);

  
  // Setup 3V comparator on channel 0
  ads1015.startComparator_SingleEnded(0, 1000);
  
}

void loop(void)
{
  int16_t adc0;

  // Comparator will only de-assert after a read
  adc0 = ads1015.getLastConversionResults();
  Serial.print("AIN0: "); Serial.println(adc0);

  //Serial.println(analogRead(A0));
  //Serial.println(analogRead(A1));

  if(toggle==1){
    switchThreshold();
  }
  
  delay(1000);

}

void indicate(){
  Serial.println("Starting interrupt");
  toggle=1;
  Serial.println("Ending interrupt");
  Serial.println("Threshold hit here");
  detachInterrupt(digitalPinToInterrupt(interruptPin)); 
}

void switchThreshold(){
  Serial.println("Calling Switch Threshold");
  ads1015.startComparator_SingleEnded(0, 1400);
  toggle = 0;
  attachInterrupt(digitalPinToInterrupt(interruptPin), indicate , RISING);

}

