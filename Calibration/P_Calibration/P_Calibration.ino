//The purpose of this subfunction is to calibrate the pressure sensor 
/* 2.23- set up structure of function, wrote mV to m function
 *  need to 1. finish reverse subfunction 
 *  2. read in data from eeprom 
 *  3. test data 
 */

#include <EEPROM.h>

const int pressurePin = A7;    // pressure sensor 


float v_power = 9.75;
const float a = 0.066466;
const float b = 0.02523;
const float m = 0.199992;
const float b2 = 0.00386;

#define SENSOR_SIZE 11
int sensor_values[SENSOR_SIZE]={0, 2, 4, 6, 8, 10, 12, 60, 80, 90, 100};

int eeAddress=0;
//depth = (10m/14.57psi)*pressure
//v_arduino = a*v_sensor + b;
//V_sensor = (v_power/10) * (m*P + b2);
//depth [m] = (10m / 14.57psi) * pressure [psi];

void setup(){

  Serial.begin(9600);
   for (int i=0; i<SENSOR_SIZE; i++){
  Serial.println(sensor_values[i]);
   } // closes for loop
}

void loop(){
   meters_to_mV();
}


void mV_to_meters(){
  //millivolts to meters (v_arduino to depth)
  //given sensor mV
  
  float pressure;
  float depth;
  float p_arduino;
  float pv_arduino;
  float pv_sensor;

  Serial.print("Read mV from array");
  
  for (int i=0; i<SENSOR_SIZE; i++){
    
  p_arduino = sensor_values[i];
  pv_arduino = (p_arduino*5)/1023; //v_arduino to pv_arduino
  pv_sensor = (pv_arduino - b)/a; //pv_arduino to pv_sensor
  pressure = ((pv_sensor*(10/v_power)-b2)/m);
  depth = (pressure)*10/14.57;
  
  Serial.println("Millivolts to meters");
  Serial.print("Millivolts: ");
  Serial.println(pv_arduino);
  Serial.print("Meters:  ");
  Serial.print(depth);
  Serial.println("");
  } //closes for loop
  
 
} // closes function 

//this subfunction reads from the SD and writes to EEPROM
void meters_to_mV(){
  //reverse of the above subfunction
  //given depth by client
  float pressure;
  float depth;
  float p_arduino;
  float pv_arduino;
  float pv_sensor;

  for (int i=0; i<SENSOR_SIZE; i++){
    pressure = ((sensor_values[i] * 14.57/10) + 14.7);
    pv_sensor = (v_power/10)*(pressure*m + b2);
    pv_arduino = pv_sensor*a + b;
    p_arduino = (pv_arduino*1023)/5;
  
    
    Serial.print("Depth: ");
    Serial.println(sensor_values[i]);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.print("PV_sensor:  ");
    Serial.println(pv_sensor);
    Serial.print("PV_arduino: ");
    Serial.println(pv_arduino);
    Serial.print("P_arduino: ");
    Serial.println(p_arduino);
  }
 
} //closes function 

