//the purpose of this sub-function is to initialize EEPROM, 
//store the global constants for key memory location in EEPROM, 
//and use sub-functions to load and store data. 

#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>

#include <DS1307RTC.h>
#include <Time.h>
#include <TimeLib.h>
#include <Wire.h>

const int chipSelect = 10;
int counter = 0;

int address = 0;
int EEsize =  1024;
byte value;

//global constants 

time_t system_start_time;
int current_time;
int number_syringes = 100;
int syringe_table_start_address;
int depth;
int current_syringe = 1;

//for loop values 
int EEstorage_lower = 0;
int EEstorage_upper = 51;

//check how many are left- only record as many syringes as present

void EEPROM_start_time(){
  EEPROM.put(address, system_start_time);
  address += sizeof(system_start_time);
}


//this part checks where the address is and correspondingly runs either the lower or upper 
//function or stops if syringes are at 250

void EEPROM_rotation(){

if(address = 4){


if(address > 300){

void EEPROM_increment_upper(){

  for ( int index = 51; index <= 100; index++)
  EEPROM.put(address, current_time);
  address += sizeof(current_time);
  EEPROM.put(address, depth);
  address += sizeof(depth);
    
}
}
}


void EEPROM_increment_lower(){
  
  for (int index = 0; index <= 50; index++) {
  
  EEPROM.put(address, current_time);
  address += sizeof(current_time);
  EEPROM.put(address, depth);
  address += sizeof(depth);
}
}



  address = address/6

  if (address = number_syringes + 4) return 0;
  
  }
    
    
  }
  
}

