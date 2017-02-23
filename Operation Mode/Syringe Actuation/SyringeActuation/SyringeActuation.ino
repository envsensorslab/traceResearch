#define syringePin1 2
#define syringePin2 3
#define syringePin3 4
#define syringePin4 5
#define syringePin5 6
#define syringePin6 7
#define syringePin7 8
#define syringePin8 9


int delayTime = 1000;
int curr_syringe = 0;
// Size of one of the sides of the mosfet matrix

// Needs to be type int, if not then this will break
const int mosfetPins[] = { 2, 3, 4, 5 };
const int selectPins[] = { 6, 7, 8, 9 };
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
  Serial.println(sizeof(selectPins));
  delay(delayTime);
}

void loop() {
  /*
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

<<<<<<< HEAD
digitalWrite(syringePin2, LOW);
=======
digitalWrite(syringePin2, LOW); */

for (int i =0; i< 16; i++)
{
  curr_syringe=i;
  Serial.println(i);
  syringe_actuation();
}
>>>>>>> origin/master
}

/*
 * Function: syringe_actuation()
 * 
 * Description: Sets the corresponding syringe pin to high to all for sampling
 * 
 * Reliance:
 * Relies on the global variable curr_syringe.
 */
void syringe_actuation()
{
  int pinHigh = 0;
  int pinLow = 0;

  Serial.println(curr_syringe/4);
  pinHigh = mosfetPins[(int)curr_syringe/4];

  /*
  for (int i =0; i<sizeof(mosfetPins); i++)
  {
    if(curr_syringe < i+sizeof(selectPins))
    {
      
    }
  }*/
  
  /*
  if(curr_syringe < 4)
  {
    pinHigh = syringePin1;
  }
  else if (curr_syringe < 8)
  {
    pinHigh = syringePin2;
  }
  else if (curr_syringe < 12)
  {
    pinHigh = syringePin3;
  }
  else if (curr_syringe < 16)
  {
    pinHigh = syringePin4;
  }*/
  Serial.print("modolo: ");
  Serial.println(curr_syringe%(sizeof(selectPins)/sizeof(int)));
  pinLow = selectPins[curr_syringe%(sizeof(selectPins)/sizeof(int))];
  /*
  if(curr_syringe%sizeof(selectPins) == 0)
  {
    pinLow = syringePin5;
  }
  else if (curr_syringe%sizeof(selectPins) == 1)
  {
    pinLow = syringePin6;
  }
  else if (curr_syringe%sizeof(selectPins) == 2)
  {
    pinLow = syringePin7;
  }
  else if (curr_syringe%sizeof(selectPins) == 3)
  {
    pinLow = syringePin8;
  }*/

  Serial.print("pinHigh is ");
  Serial.println(pinHigh);
  Serial.print("PinLow is ");
  Serial.println(pinLow);
  digitalWrite(pinHigh, HIGH);
  digitalWrite(pinLow, HIGH);
  delay(2000);
  digitalWrite(pinHigh, LOW);
  digitalWrite(pinLow, LOW);  
}

