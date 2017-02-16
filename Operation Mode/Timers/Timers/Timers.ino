//pumping loop
//A3 is pump enable 
//A2 is pump direction

#include <TimerOne.h>
//these LEDs are for testing purposes
int pumpDirection = A2; //green LED
int pumpEnable= A3; //blue LED


int wait=0;

//testing the timer 
long startTime;
long elapsedTime;

long forward_time  = 4000000;
long backward_time = 4000000;



void setup() {
  Serial.begin(9600);
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
  // Timer1.initialize(4000000); // set a timer of length 4000000 microseconds (or 4 sec)
  Timer1.attachInterrupt(isrDirection);
  /*noInterrupts();
  //disables interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  
  //prescale the timer to 4s
  int  timer1_counter = 15624;
  TCNT1 = timer1_counter;
  TCCR1B |= (1 << CS12); //256 prescaler 
  TIMSK1 |= (1 << TOIE1); //enable timer overflow interrupt 
  interrupts(); //reenable interrupts 
  */
  
}

void loop() {

  wait=0;
  Serial.println("Now running forward pump sequence");
  pump_sequence_forward();
  while (wait==0){
  Serial.println("Waiting on backward sequence");
  } //while loop closed
  pump_sequence_backward();
  Serial.println("Pump sequence complete");
  Serial.println("");
  
  

}

void pump_sequence_forward(){
  //set pin direction forward (set to high)
  Timer1.initialize(forward_time);  //initialize timer
  Serial.println("Set pin direction forward");
  digitalWrite(A2, HIGH);
  // Serial.println("Start delay");
  delay(500);
  //Serial.println("End delay");
  
  
//start timer 
  //RTC timer 
  //Serial.println("Start timer");
  //startTime = millis();
  
  
//run pump sequence
  //set pump enable to high
  Serial.println("Set pump enable high");
  digitalWrite(A3, HIGH);
  delay(500);
  

//timer ends 
  //set pump enable to low
  //Serial.println("Set pump enable low"); 
  //digitalWrite(A3, LOW);

  //Serial.println(elapsedTime);
  Serial.println();

}

void pump_sequence_backward(){
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low

  Serial.println("Read pressure and temp here");
  //sensor_inputs(); //call this subfunction to read pressure and temperature here

 
  Timer1.initialize(backward_time);  //initialize timer
  Serial.println("Now running backward pump sequence");
  Serial.println("Set direction pin low");
  digitalWrite(A2, LOW);
   Serial.println("Set pump enable pin high- start pumping");
  digitalWrite(A3, HIGH);
  
  
  //elapsedTime = millis()-startTime;
  //Serial.println("Start timing");
  delay(500);
  Serial.println();
  
  
}
void isrDirection(){
  Serial.println("Interrupt starting");
  wait = 1;
  Serial.println("Set pump enable pin low- stop pumping");
  digitalWrite(A3, LOW);
  Timer1.stop();
  Serial.println("Interrupt finished");
}



