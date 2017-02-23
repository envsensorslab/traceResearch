//pumping loop
//A3 is pump enable 
//A2 is pump direction

//these LEDs are for testing purposes
#define pumpDirection A2 //green LED
#define pumpEnable A3 //blue LED


boolean pumpOn = false;
//pumpForw == true -> forward direction
//pumpForw == false -> reverse direction
boolean pumpForw = true;

int forward_flush_time = 5000;
int reverse_flush_time = 2000;
int pump_start_time = 2000;



void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);  
}

void loop() {
  // Start pumping in the forward direction
  sampleSequence();
}

void sampleSequence()
{
  pump_sequence_forward();  
  //syringeActuation();
  // Start pumping in the reverse direction
  pump_sequence_backward();
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


