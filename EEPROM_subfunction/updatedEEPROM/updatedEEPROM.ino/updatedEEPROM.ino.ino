#include <EEPROM.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <TimeLib.h>
#include <Wire.h>
#include <SD.h>


int address = 0;
int EEsize = 512;
byte value;
const int s_rotation = 49;
int depth;
time_t collection_time;
int lower = 49;
int upper = 99;
# define SIZE_OF_SYRINGE 6
int curr_sryinge = 1;

File sampleDataTableFile;
char sampleDataTableName[] = "sampleTB.csv";



//global constants

//case 1- values that are stored once at initialization

time_t system_start_time = 500;
int number_syringes = 250;
int curr_syringe = 0;
#define FLUSH_EEPROM_ADDRESS 10
#define SYRINGE_ADDRESS 200

void setup(){
   EEPROM_setup();
     
}

void loop(){
  
}

void EEPROM_setup(){
  EEPROM.write(address, system_start_time);
  address = address + 4;
  EEPROM.write(address, number_syringes);
  }

//need to figure out how to advance to next address based on size of these variables

//case 2- changing syringe values

void syringeIteration(){
  //upper lower argument
  
  int i=0;
  int counter = 0;
  File sampleDataTableFile = SD.open(sampleDataTableName);
  // If we were able to open up the files
  if (sampleDataTableFile)
  {
    if (counter >= number_syringes){
      return 0; 
    }
    
    int start;
    int finish;
    
    if ( curr_sryinge < 50 )
    {
      //in upper
       start = 0;
       finish = 49;
    }
    else
    {
      //in lower
       start = 50;
       finish = 99;
    }
   
    for (int i= start; i<=finish; i++){
  
      //get to row start
      //read in row start
      // readVals()
      //read from sensors
      // SYRINGE_ADDRESS = startSryinges * 6(counter)
      EEPROM.write(SYRINGE_ADDRESS, depth);
     
    
      EEPROM.write(SYRINGE_ADDRESS + 4, collection_time);
      
      SYRINGE_ADDRESS == SYRINGE_ADDRESS + SIZE_OF_SYRINGE;
    
      counter++;
    
    }
  
  }
}



bool readLine(File &f, char* line, size_t maxLen) {
  
  for (size_t n = 0; n < maxLen; n++) {
    int c = f.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}
bool readVals(int* v1, int* v2) {
  char line[40], *ptr, *str;
  if (!readLine(sampleDataTableFile, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v2 = strtol(ptr, &str, 10);
  return str != ptr;  // true if number found
}

