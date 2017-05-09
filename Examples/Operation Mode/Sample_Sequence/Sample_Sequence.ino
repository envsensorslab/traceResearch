//pumping loop
//A3 is pump enable 
//A2 is pump direction

#define syringePin1 2
#define syringePin2 3
#define syringePin3 4
#define syringePin4 5
#define syringePin5 6
#define syringePin6 7
#define syringePin7 8
#define syringePin8 9


//these LEDs are for testing purposes
#define pumpDirection A2 //green LED
#define pumpEnable A3 //blue LED

//Pin definition
#define mosfestNumPins 4
const int mosfetPins[] = { syringePin1, syringePin2, syringePin3, syringePin4 };
#define selectNumPins 4
const int selectPins[] = { syringePin5, syringePin6, syringePin7, syringePin8 };


boolean pumpOn = false;
//pumpForw == true -> forward direction
//pumpForw == false -> reverse direction
boolean pumpForw = true;

int forward_flush_time = 3000;
int reverse_flush_time = 1000;
int pump_start_time = 2000;
int curr_syringe = 0;



void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  initPump();
  initSyringes();
}

void loop() {
  // Start pumping in the forward direction
  sampleSequence();
}

void sampleSequence()
{
  pump_sequence_forward();  
  syringe_actuation();
  // Start pumping in the reverse direction
  pump_sequence_backward();
  curr_syringe++;
}

/*
 * Function: initPump()
 * 
 * Description: Intalizes pump pins to output
 */
void initPump()
{
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
}

/*
 * Function: pump_sequence_forware()
 * 
 * Description: Starts the pump in the forward direction for the specified amount of time
 *  from the EEPROM
 *  
 *  Relies:
 *    forward_flush_time: which is a global variable read in from the EEPROM
 */
void pump_sequence_forward(){
  Serial.println("Start forward sequence");

  pumpForw = true;
  pumpOn = true;  
  updatePumpState(pumpOn,pumpForw);
  Serial.println("starting delay");
  delay(pump_start_time);
  delay(forward_flush_time);
  pumpOn= false;
  updatePumpState(pumpOn, pumpForw);
}

/*
 * Function: pump_sequence_backward()
 * 
 * Description: Starts the pum in the reverse direction for the specified amount of time
 * 
 * Relies:
 *  reverse_flush_time: which is a global variable read in from the EEPROM
 */
void pump_sequence_backward(){
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low
  Serial.println("Starting backup function");
  pumpForw = false;
  pumpOn = true;
  updatePumpState(pumpOn, pumpForw);
  delay(pump_start_time);
  delay(reverse_flush_time);
  pumpOn= false;
  updatePumpState(pumpOn, pumpForw);
}

/*
 * Function: updatePumpState(boolean pumpPower, boolean pumpDLR)
 * 
 * Description: Updates the current state of the pump. It is passed in the direction the pump should be 
 *  and also if the pump should be enabled. 
 *  
 *  Arguments:
 *    boolean pumpPower: True or False to indicate whether the pump enable should occur
 *    boolean pumpDLR: True or False to indicate the direction the pump should pump
 *      True -> Forward direction
 *      False -> Reverse direction
 *      
 *  Relies:
 *    The pin numbers for the pumpDirection and the pumpEnable
 */
void updatePumpState(boolean pumpPower, boolean pumpDLR){
  Serial.println("updating pump state");
  pumpForw = pumpDLR;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = pumpPower;
  digitalWrite(pumpEnable, pumpOn);
}

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
  //LogPrint(SYSTEM, LOG_INFO, F("Done with init of syringe pins"));
  delay(500);
}



