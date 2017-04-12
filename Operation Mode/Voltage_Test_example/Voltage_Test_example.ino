
int togglePin = A1;
int state = HIGH;
int reading;
int previous = LOW;
long time = 0;         // the last time the output pin was toggled


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(togglePin, OUTPUT);

}

void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  Serial.println(voltage);
  
 //unsigned long currTime = millis();
 

 /*
 //delay(3000);
 for (int i=0; i<100; i++){
  Serial.print(voltage);
  Serial.print("  ");
  Serial.println (currTime);
 }
 digitalWrite(togglePin, HIGH);
 for (int i=0; i<100; i++){
  Serial.print(voltage);
  Serial.print("  ");
  Serial.println (currTime);
 }
 Serial.println("Switched");
 //delay(3000);
 for (int i=0; i<100; i++){
  Serial.print(voltage);
  Serial.print("  ");
  Serial.println (currTime);
 }
 digitalWrite(togglePin, LOW);
 for (int i=0; i<100; i++){
  Serial.print(voltage);
  Serial.print("  ");
  Serial.println (currTime);
 }
 Serial.println("Switched");
 */

 delay (100);


  
}

//serial plotter tool 
