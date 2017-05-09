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
#define mosfestNumPins 4
const int mosfetPins[] = { syringePin1, syringePin2, syringePin3, syringePin4 };
#define selectNumPins 4
const int selectPins[] = { syringePin5, syringePin6, syringePin7, syringePin8 };
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // Initalizes all the syringe pins as outputs and makes sure they are set low
  initSyringes();
}

void loop() {
 
  for (int i =0; i< 16; i++)
  {
    curr_syringe=i;
    Serial.println(i);
    syringe_actuation();
  }
}

/*
 * Function: initSyringes()
 * 
 * Description: Sets all the syringe pins as outputs and make sure they are set low
 * 
 */
void initSyringes()
{
  for (int i =0; i< (mosfestNumPins + selectNumPins); i++)
  {
    //First set all the mosfet pins
    //Mosfet pins are the postive pin that selects the correct J component. (connected to PNP type Mosfet)
    if(i < mosfestNumPins)
    {
       Serial.print("Mosfet: ");
       Serial.println(mosfetPins[i]);
       pinMode(mosfetPins[i], OUTPUT);
       digitalWrite(mosfetPins[i], LOW);  
    }
    // make sure to set all the selector pins. Selector pins select which syringe inside
    // of each J componenent piece. (connected to NPN type mosfet
    else 
    {
       Serial.print("selector: ");
       Serial.println(selectPins[i-mosfestNumPins]);
       pinMode(selectPins[i-mosfestNumPins], OUTPUT);
       digitalWrite(selectPins[i-mosfestNumPins], LOW);  
    }
    
  }
  //Make sure the ports have time to settle
  delay(500);
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

  Serial.print("modolo: ");
  Serial.println(curr_syringe%(mosfestNumPins));
  pinLow = selectPins[curr_syringe%(selectNumPins)];
 
  Serial.print("pinHigh is ");
  Serial.println(pinHigh);
  Serial.print("PinLow is ");
  Serial.println(pinLow);
  digitalWrite(pinHigh, HIGH);
  digitalWrite(pinLow, HIGH);
  //Need to figure out the delay for actuation the syringe
  delay(2000);
  digitalWrite(pinHigh, LOW);
  digitalWrite(pinLow, LOW);  
}

