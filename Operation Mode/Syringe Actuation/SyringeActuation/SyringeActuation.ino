#define syringePin1 2
#define syringePin2 3
#define syringePin3 4
#define syringePin4 5
#define syringePin5 6
#define syringePin6 7
#define syringePin7 8
#define syringePin8 9

int delayTime = 1000;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  for (int i =0; i< 8; i++)
  {
    Serial.println("hi");
    digitalWrite(i+2, LOW);
    pinMode(i+2, OUTPUT);
  }
  delay(1000);
}

void loop() {
//testing loop to set pin 1 and every other pin high

//syringe set 1-4
digitalWrite(syringePin1,HIGH);

//syringe 1
digitalWrite(syringePin5,HIGH);
delay(delayTime);
digitalWrite(syringePin5,LOW);

//syringe 2
digitalWrite(syringePin6,HIGH);
delay(delayTime);
digitalWrite(syringePin6,LOW);

//syringe 3
digitalWrite(syringePin7,HIGH);
delay(delayTime);
digitalWrite(syringePin7,LOW);

//syringe 4
digitalWrite(syringePin8,HIGH);
delay(delayTime);
digitalWrite(syringePin8,LOW);

digitalWrite(syringePin1,LOW);


//next set
digitalWrite(syringePin2, HIGH);

//syringe 5
digitalWrite(syringePin5,HIGH);
delay(delayTime);
digitalWrite(syringePin5,LOW);

//syringe 6
digitalWrite(syringePin6,HIGH);
delay(delayTime);
digitalWrite(syringePin6,LOW);

//syringe 7
digitalWrite(syringePin7,HIGH);
delay(delayTime);
digitalWrite(syringePin7,LOW);

//syringe 8
digitalWrite(syringePin8, HIGH);
delay(delayTime);
digitalWrite(syringePin8, LOW);

digitalWrite(syringePin2, LOW);


 
}
