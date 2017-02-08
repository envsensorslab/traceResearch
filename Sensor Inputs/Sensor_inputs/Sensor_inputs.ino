/*Sensor Inputs
 * read from each sensor attached to a pin 
 * Pressure A7
 * Temperature A6
 * LED A1 on redboard for demo
 */

int sensorMin = 1023;
int sensorMax = 0;
const int pressurePin = 7;
//const int temperaturePin = 6;  for the actual minipro
const int temperaturePin = 0;    //for testing purposes on the redboard
int sensorValue = 0;

void setup() {
 Serial.begin(9600);
}

void loop() {
  int temperature;
  temperature = getVoltage(temperaturePin);
  //pressure = getVoltage(pressurePin);
  Serial.println(temperature);
  //int degreesC = ((temperature*0.004882814)-0.5)*100.0;  //for later on to convert to degrees
  Serial.println(degreesC); 
  delay (1000);
}




int getVoltage(int pin){
// return(analogRead(pin)*0.004882814);
return(analogRead(pin));
}

