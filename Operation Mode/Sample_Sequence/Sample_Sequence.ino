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
  Serial.begin(9600);
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
  
}

void loop() {


  Serial.println("Starting forward pump sequence");
  pump_sequence_forward();
 
  delay(forward_time);
  
  Serial.println("Read pressure and temp here");
  //sensor_inputs(); //call this subfunction to read pressure and temperature here
  //String output = "";
  //output = "Rev flush: " + (String)[curr_syringe, pressure, temperature];
  //LogPrint(SYSTEM, LOG_INFO, output.c_str());                   
  
  //sub-function for syringe actuation
  //syringe_actuation(curr_syringe)

  updatePumpState(true,false);
  pump_sequence_backward();
  Serial.println("Pump sequence complete");
  Serial.println("");
}


// sub-functions //////////////
void pump_sequence_forward(){

  Serial.println("Set pin direction forward");
  pumpForw = true;

  Serial.println("Running forward pump sequence...");
  digitalWrite(pumpDirection,pumpForw);
  pumpOn = true;
  digitalWrite(pumpEnable,pumpOn);

  Serial.println("Stopping pump");
  pumpOn= false;
  digitalWrite(pumpEnable, pumpOn);

}

void pump_sequence_backward(){
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low

  Serial.println("Now running backward pump sequence");
  Serial.println("Set direction pin low");
  pumpForw = false;
  digitalWrite(pumpDirection, pumpForw);
  Serial.println("Set pump enable pin high- start pumping");
  pumpOn = true;
  digitalWrite(pumpEnable, pumpOn);

  Serial.println("Stopping pump");
  pumpOn= false;
  digitalWrite(pumpEnable, pumpOn);
 
  delay(5000);
  Serial.println();
  

}

void updatePumpState(boolean pumpPower, boolean pumpDLR){
  pumpForw = pumpDLR;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = pumpPower;
  digitalWrite(pumpEnable, pumpOn);
}


