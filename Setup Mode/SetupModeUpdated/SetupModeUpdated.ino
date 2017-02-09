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
#include <time.h>

File sampleDataTableFile;
File configFile;
RTC_DS3231 rtc;

char sampleDataTableName[] = "sampleTB.csv";
char configFileName[] = "configFl.txt";

#define SIZE_OF_SYRINGE 6
#define system_start_time_address 0
#define number_syringes_address 4
#define curr_syringe_address 6
#define syring_table_start_address 8
#define forward_flush_time_address 10

int curr_syringe = 0;
int number_syringes = 0;
int syringe_table_start = 0;


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


  initPeripherals();  
  curr_syringe = 0;
  syringeIteration();
  curr_syringe = 50;
  syringeIteration();
  
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
  }
}

void initEEPROM()
{
  // Open the configuration file  
    if (SD.exists(configFileName))
    {
      configFile = SD.open(configFileName, FILE_READ);
      if(configFile)
      {       
        int counter = 0;        
        char line[40];
        while(configFile.available()) 
        {     
          readLine(configFile,line, sizeof(line));
          String toString = String(line);
          //Value is long int because time_t is long int
          long int value = toString.toInt();
          String output = "";
          // Load forward flush time
          if (counter == 0)
          {
            EEPROM.put(system_start_time_address, (time_t)value);
            output = "System state time value: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 1)
          {
            EEPROM.put(number_syringes_address, value);
            number_syringes = value;
            output = "Num of Syringes: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 2)
          {
            EEPROM.put(curr_syringe_address, value);
            output = "Curr Syringe set to : " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 3)
          {
            //EEPROM.put(syring_table_start_address, value);
            syringe_table_start = value;
            output = "Syringe table start address: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 4)
          {            
            // Load reverse flush time
            EEPROM.put(forward_flush_time_address, value);
            output = "Forward flush time set to: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
            
          }
           counter++;
          
    
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
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //Since this is the setup routine, always set the RTC value
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.print("RTC value equals: ");
  String t ="";
  timestamp(t);
  Serial.println(t);
}

void initPeripherals()
{
  initSDcard();
  initRTC();  
  initEEPROM();
}

// Loads the upper and lower half dependent on where the current syringe value is
void syringeIteration(){  
  LogPrint(SYSTEM, LOG_INFO, "Starting syringeIteration");
  int counter = 0;
  if(SD.exists(sampleDataTableName))
  {
    sampleDataTableFile = SD.open(sampleDataTableName);
    if (sampleDataTableFile)
    { 
      Serial.println("here");
      if (curr_syringe >= number_syringes){
        return; 
      }
      
      int start;
      int finish;
      // Determines whether it is in the upper or lower section by where
      // the current syringe is
      // Functionality: When the syringe hits spot 0, it will populate it's current section
      // AKA when current syringe hits 100, it will populate the EEPROM with data from 100-149
      Serial.println(curr_syringe%100);
     
      if ( curr_syringe%100 < 50)
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

      // Start iterating through until the line we want to start recording
      int i=0;
      while (i<curr_syringe){
        long int a=0;
        int b=0;
        Serial.println("while loop");
        readVals(&a,&b);  
        i++;      
      }
      // Once we are at the right spot start recording to the EEPROM
      for (int i= start; i<=finish; i++)
      {

        //If there is no more data to read in then also quit function
        if(!sampleDataTableFile.available())
        {
          break;
        }
        
        long int x=0;
        int y=0;        
        readVals(&x,&y);            
        time_t samTime = x;
        
        Serial.println(i);        
        Serial.print("x: ");
        Serial.println(samTime);
        Serial.print("y: ");
        Serial.println(y);
        Serial.print("Address = :");
        Serial.println(syringe_table_start + (i*SIZE_OF_SYRINGE));        
        Serial.println();

        // Put the csv values into the EEPROM
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE), samTime);
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE) + 4, y);

        
      }
      sampleDataTableFile.close();
    }
    else
    {
      LogPrint(SYSTEM, LOG_ERROR, "Could not open Sample Data File");
    }
  }
  else
  {
    LogPrint(SYSTEM, LOG_ERROR, "Sample Data table File does not exist");
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

bool readVals(long int* v1, int* v2) {
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






