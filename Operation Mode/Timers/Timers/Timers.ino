//pumping loop
//A3 is pump enable 
//A2 is pump direction

#include <TimerOne.h>
int pumpDirection = A2; //green LED
int pumpEnable= A3; //blue LED
//these LEDs are for testing purposes
int pumpDirectionLED = 8;
int pumpEnableLED = 9;

//testing the timer 
long startTime;
long elapsedTime;



void setup() {
  Serial.begin(9600);
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( direction_timer );

  noInterrupts();
  //sets TCCR1A and TCCR1B registers to 0
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0x0BDC;
  //prescale the timer to 4s
  //OCR1A = 31250;         //sets compare match register to desired timer count
  TCCR1B|= (1 << WGM12); //sets TCCR1B timer period to 256 Hz
  TCCR1B|= (1 << CS12);
  TIMSK1|= (1 << OCIE1A);

  interrupts();
}

void loop() {

  Serial.println("Now running forward pump sequence");
  pump_sequence_forward();
  pump_sequence_backward();
  Serial.println("Pump sequence complete");
  

}

void pump_sequence_forward(){
  //set pin direction forward (set to high)
  Serial.println("Set pin direction forward");
  digitalWrite(A2, HIGH);
  Serial.println("Start delay");
  delay(10);
  Serial.println("End delay");
  
  
//start timer 
  //RTC timer 
  Serial.println("Start timer");
  startTime = millis();
  
  
//run pump sequence
  //set pump enable to high
  Serial.println("Set pump enable high");
  digitalWrite(A3, HIGH);
  delay(10);
  


//reverse pump sequence and run 
  //set pump enable to high
  Serial.println("Set pump enable high");
  digitalWrite(A3, HIGH);
  delay(10);
  

//timer ends 
  //set pump enable to low
  Serial.println("Set pump enable low"); 
  digitalWrite(A3, LOW);

  Serial.println(elapsedTime);
  Serial.println();

}

void pump_sequence_backward(){
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low
  direction_timer();
  Serial.println("Now running backward pump sequence");
  Serial.println("Set direction pin low");
  digitalWrite(A2, LOW);
  Serial.println("Set pump enable pin low");
  digitalWrite(A3, LOW);
  Serial.println("Read pressure and temp here");
  //sensor_inputs(); //call this subfunction to read pressure and temperature here
  elapsedTime = millis()-startTime;
  Serial.println("Start timing");
  delay(10);
  Serial.println();
  
  
}
void direction_timer(){
  digitalWrite(A2, digitalRead(A2)^1);
}



