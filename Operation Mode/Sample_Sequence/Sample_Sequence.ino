//pumping loop
//A3 is pump enable 
//A2 is pump direction

//these LEDs are for testing purposes
int pumpDirection = A2; //green LED
int pumpEnable= A3; //blue LED


boolean pumpOn = false;
//pumpForw == true -> forward direction
//pumpForw == false -> reverse direction
boolean pumpForw = true;

byte forward_flush_time = 5000;
byte reverse_flush_time = 2000;



void setup() {
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);  
}

void loop() {
  // Start pumping in the forward direction
  pump_sequence_forward();  
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

  pumpForw = true;
  pumpOn = true;  
  updatePumpState(pumpOn,pumpForw);
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

  pumpForw = false;
  pumpOn = true;
  updatePumpState(pumpOn, pumpForw);
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
  pumpForw = pumpDLR;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = pumpPower;
  digitalWrite(pumpEnable, pumpOn);
}


