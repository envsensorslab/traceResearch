#include <EEPROM.h>
#include <DS1307RTC.h>
#include <SD.h>
#include <SPI.h>


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
byte shift = 4;

File sampleDataTableFile;
char sampleDataTableName[] = "sampleTB.csv";



//global constants

//case 1- values that are stored once at initialization

time_t system_start_time = 500;
int number_syringes = 250;
int curr_syringe = 0;
char data_array [250];
#define FLUSH_EEPROM_ADDRESS 10
#define SYRINGE_ADDRESS 200

void setup(){
   EEPROMsetup();

   syringeIteration();
   
     
}

void loop(){
  
}

void EEPROMsetup(){
  EEPROM.put(address, system_start_time);
  address = address + 4;
  EEPROM.put(address, number_syringes);
  }

//need to figure out how to advance to next address based on size of these variables

//case 2- changing syringe values

void syringeIteration(){
  //upper lower argument
  
  int i=0;
  int counter = 0;
  File  logFile;
  logFile = SD.open("sampleDataTableFile", FILE_READ);
  
  // If we were able to open up the files
  if (logFile)
  {
    if (curr_syringe >= number_syringes){
      return 0; 
    }
    
    int start;
    int finish;
    
    if ( curr_sryinge%100 < 50 )
    {
      //in lower 
       start = 0;
       finish = 49;
    }
    else
    {
      //in upper
       start = 50;
       finish = 99;
    }
    while (i<curr_syringe){
    int a=0;
    int b=0;
      readVals(a,b);
      
    }
    for (int i= start; i<=finish; i++){
      int x=0;
      int y=0;
      //get to row start
      //read in row start
      //readVals()
      //read from sensors
      // SYRINGE_ADDRESS = startSryinges * 6(counter)
    
   
      readVals(x,y);
      
      EEPROM.put(SYRINGE_ADDRESS + (i*SIZE_OF_SYRINGE), x);
      EEPROM.put(SYRINGE_ADDRESS + (i*SIZE_OF_SYRINGE) + shift, y);
 
  }
  }

  logFile.close();
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

