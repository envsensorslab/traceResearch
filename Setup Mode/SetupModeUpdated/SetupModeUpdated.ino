/*
  SD card basic file example

 This example shows how to create and destroy an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */
#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

File sampleDataTableFile;

char sampleDataTableName[] = "sampleTB.csv";
char configFileName = "configFl.txt";

enum error_level {
  LOG_ERROR,
  LOG_WARNING,
  LOG_DEBUG,
};

enum module {
  DATA,
  SYSTEM,
};

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  initPeripherals();  
  setupRoutine();
  
  LogPrint(DATA, LOG_WARNING, "Hi therer");
  LogPrint(SYSTEM, LOG_ERROR, "BYe there");
}

void loop() {
  // nothing happens after setup finishes.
}

void initSDcard()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //Opens a file, then tries to remove it, if it works then SD card init sucessful
  File exampleFile = SD.open("example.txt");
  if (SD.exists("example.txt")) 
  {   
    // Remove the file once the data is loaded
    SD.remove("example.txt");
    if (SD.exists("example.txt")) 
    {
      Serial.println("SD card init failed");
    }    
    else{
      Serial.println("SD card init succesfully");
    }
  } 
  else 
  {
    Serial.println("SD card init failed");
  }
}

void initEEPROM()
{
  // Open the configuration file  
    File configFile = SD.open(configFileName);
    if(configFileName)
    {         
      // Load forward flush time
      // Load reverse flush time

      // Load data to EEPROM
    }
}

void initPeripherals()
{
  initSDcard();
  initEEPROM();
  loadConfigVars();
}


void setupRoutine()
{
  
  int num_load_syringes = 1;
  int total_num_syringes = 1;
  if(SD.exists(sampleDataTableName))
  {
    File sampleDataTableFile = SD.open(sampleDataTableName);
    if (sampleDataTableFile)
    {
      
    int x,y;
    // Read in each value
    while (readVals(&x, &y)) 
    {
      Serial.print("x: ");
      Serial.println(x);
      Serial.print("y: ");
      Serial.println(y);
      Serial.println();

      // Only want to load 100 syringe data points, keep looping to see total number of syringes though
      if (num_load_syringes <= 100)
      {        
        //Store value in EEPROM
        Serial.print("Loading syring #"); Serial.println(num_load_syringes);
        num_load_syringes++;
      }
      total_num_syringes++;      
    }
      //Write total num and load num to EEPROM
      
      sampleDataTableFile.close();
    } else {
      Serial.println("Error opening sampleFile.csv");
    }
  } else {
    Serial.println("File does not exist");
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


void LogPrint(module moduleName, error_level errorLevel, char logData[])
{
  File logFile;
  if (moduleName == DATA)
  {    
    logFile = SD.open("data.txt", FILE_WRITE);
  }
  else if (moduleName == SYSTEM)
  {
    logFile = SD.open("system.txt", FILE_WRITE);
  }
  if (logFile)
  {
    Serial.println("File exists");
    String level;
    if (errorLevel == LOG_ERROR)
    {
      level="Error";
    }
    else if (errorLevel == LOG_WARNING)
    {
      Serial.println("Warning");
      level="Warning";
    }
    else 
    {
      Serial.println("Debug");
      level="Debug";
    }
    
    // Switch to reading from the RTC once it is integrated
    time_t t = now();
    String myStr = t + ", " + level + ", "+ logData;
    logFile.println(myStr);
    Serial.println(myStr);    
    logFile.close();
  }
}

