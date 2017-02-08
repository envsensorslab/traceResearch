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
#include "RTClib.h"
#include <EEPROM.h>

File sampleDataTableFile;
RTC_DS3231 rtc;

char sampleDataTableName[] = "sampleTB.csv";
char configFileName[] = "configFl.txt";

#define system_start_time_address 0
#define number_syringes_address 4
#define syring_table_start_address 6
#define forward_flush_time_address 8

enum error_level {
  LOG_ERROR,
  LOG_WARNING,
  LOG_INFO,
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

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //Since this is the setup routine, always set the RTC value
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


  initPeripherals();  
  setupRoutine();
  
  LogPrint(DATA, LOG_WARNING, "Log Test 1, Warning");
  LogPrint(SYSTEM, LOG_ERROR, "Log Test 2, Error");
}

void loop() {
  // nothing happens after setup finishes.
}

void initSDcard()
{
  Serial.print("Initializing SD card...");

  // CHange back to 10 for Pro mini
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
/*
  //Opens a file, then tries to remove it, if it works then SD card init sucessful
  File exampleFile = SD.open("example.txt", FILE_WRITE);
  if (SD.exists("example.txt")) 
  {   
    // Remove the file once the data is loaded
    SD.remove("example.txt");
    if (SD.exists("example.txt")) 
    {
      Serial.println("SD card init failed, couldn't remove file");
    }    
    else{
      Serial.println("SD card init succesfully");
    }
  } 
  else 
  {
    Serial.println("SD card init failed, couldn't open file");
  }
  if(exampleFile)
  {
    exampleFile.close();
  }*/
}

void initEEPROM()
{
   Serial.println("start");
  // Open the configuration file  
    if (SD.exists(configFileName))
    {
      File configFile = SD.open(configFileName, FILE_READ);
      if(configFile)
      {       
        int counter = 0;
        
        String line;
        while(configFile.available()) 
        {     
          line = configFile.read();
          Serial.println(line);
          // Load forward flush time
          if (counter == 0)
          {
           // EEPROM.put(system_start_time_address, line);
          }
          else if (counter == 1)
          {
            //EEPROM.put(syring_table_start_address, line);
          }
           counter++;
          // Load reverse flush time
          
    
          // Load data to EEPROM
        }
        configFile.close();
      }
    }
    else
    {
      File configFile = SD.open(configFileName, FILE_READ);
      if(configFile)
      {
        configFile.close();
      }
      Serial.println("Could not find config file");
    }
}

void initRTC()
{
  Serial.print("RTC value equals: ");
  String t ="";
  timestamp(t);
  Serial.println(t);
}

void initPeripherals()
{
  initSDcard();
  initRTC();  
  //initEEPROM();
  //loadConfigVars();
}


void setupRoutine()
{
  Serial.println("starting routine");
  int num_load_syringes = 1;
  int total_num_syringes = 1;
  if(SD.exists(sampleDataTableName))
  {
    sampleDataTableFile = SD.open(sampleDataTableName);
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
    Serial.println("Sample Data Table does not exist");
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
    String level;
    if (errorLevel == LOG_ERROR)
    {
      level="Error";
    }
    else if (errorLevel == LOG_WARNING)
    {
      level="Warning";
    }
    else if (errorLevel == LOG_INFO)
    {
      level="Info";
    }
    else 
    {
      level="Debug";
    }
    
    // Switch to reading from the RTC once it is integrated
    String t = "";
    timestamp(t);
    String myStr = t + ", " + level + ", "+ logData;
    logFile.println(myStr);
    Serial.println(myStr);    
    logFile.close();
  }
}

void timestamp(String &timeFormat)
{
    DateTime now = rtc.now();
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}

