//The purpose of this subfunction is to calibrate the pressure sensor 
/* 2.23- set up structure of function, wrote mV to m function
 *  need to 1. finish reverse subfunction 
 *  2. read in data from eeprom 
 *  3. test data 
 */

#include <EEPROM.h>

const int pressurePin = A7;    // pressure sensor 


int v_power = 9.75;
const int a = 0.0014705;
const int b = 0.0002378;
const int m = 0.199992;
const int b2 = 0.00386;

int eeAddress=0;
//depth = (10m/14.57psi)*pressure
//v_arduino = a*v_sensor + b;
//V_sensor = (v_power/10) * (m*P + b2);
//depth [m] = (10m / 14.57psi) * pressure [psi];

void setup(){

  Serial.begin(9600);
  
}

void loop(){

 mV_to_meters();
 meters_to_mV();
}


void mV_to_meters(){
  //millivolts to meters (v_arduino to depth)
  //given sensor mV
  
  int pressure;
  int depth;
  int p_arduino;
  int pv_arduino;
  int pv_sensor;

  Serial.print("Read mV from EEPROM");
  pv_arduino = (p_arduino*5)/1023; //v_arduino to pv_arduino
  pv_sensor = (pv_arduino - b)/a; //pv_arduino to pv_sensor
  pressure = ((pv_sensor*(10/v_power)-b2)/m);
  depth = (pressure-14.7)*10/14.57;
  
  
  Serial.println("Millivolts to meters");
  Serial.print("Millivolts: ");
  Serial.println(pv_arduino);
  Serial.print("Meters:  ");
  Serial.print(depth);
  Serial.println("");
}
 
void meters_to_mV(){
  //reverse of the above subfunction
  //given depth by client
  int pressure;
  int depth;
  int p_arduino;
  int pv_arduino;
  int pv_sensor;
  
  pressure = depth * 14.57/10);
  pv_sensor = (v_power/10)*(pressure*m + b2);
  pv_arduino = pv_sensor*a + b;
  p_arduino = (pv_arduino*1023)/5;

  Serial.println("Depth");
  Serial.print(depth);
  Serial.println("Pressure: ");
  Serial.println(pressure);
  Serial.print("PV_sensor:  ");
  Serial.print(pv_sensor);
  Serial.println("PV_arduino");
  Serial.print(pv_arduino);
  Serial.println("P_arduino");
  Serial.print(p_arduino);
  
  
}

