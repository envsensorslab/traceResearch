//pumping loop
//A3 is pump enable 
//A2 is pump direction

//these LEDs are for testing purposes
int pumpDirection = A2; //green LED
int pumpEnable= A3; //blue LED


boolean pumpOn = false;
boolean pumpForw = true;

int forward_time = 5000*2;



void setup() {
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
  
}

void loop() {


  pump_sequence_forward();
 
  delay(forward_time);
  
  //sensor_inputs(); //call this subfunction to read pressure and temperature here
  //String output = "";
  //output = "Rev flush: " + (String)[curr_syringe, pressure, temperature];
  //LogPrint(SYSTEM, LOG_INFO, output.c_str());                   
  
  //sub-function for syringe actuation
  //syringe_actuation(curr_syringe)

  updatePumpState(true,false);
  pump_sequence_backward();
}


// sub-functions //////////////
void pump_sequence_forward(){

  pumpForw = true;

  digitalWrite(pumpDirection,pumpForw);
  pumpOn = true;
  digitalWrite(pumpEnable,pumpOn);

  pumpOn= false;
  digitalWrite(pumpEnable, pumpOn);

}

void pump_sequence_backward(){
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low

  pumpForw = false;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = true;
  digitalWrite(pumpEnable, pumpOn);

  pumpOn= false;
  digitalWrite(pumpEnable, pumpOn);

  

}

void updatePumpState(boolean pumpPower, boolean pumpDLR){
  pumpForw = pumpDLR;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = pumpPower;
  digitalWrite(pumpEnable, pumpOn);
}


