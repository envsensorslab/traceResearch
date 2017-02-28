/*
  Layout is meant for Arudino Pro Mini

 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
 * 
 * RTC is attached to I2C as follows:
 * SDA - pin A5
 * SCL - pin A4

 created  January 2017
 by Alex Agudelo & Fizzah Shaikh
 */
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <time.h>
#include <avr/pgmspace.h>
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

File sampleDataTableFile;
File exampleFile;
File configFile;

char configFileName[] = "configFl.txt";
char sampleDataTableName[] = "sampleTB.csv";
char dataLogFile[] = "data.csv";
char systemLogFile[] = "system.txt";
char exampleFileName[] = "example.txt";
char stateLogFile[] = "state.txt";
RTC_DS3231 rtc;

// Config define varaibles
#define SIZE_OF_SYRINGE 6
#define SIZE_OF_STRING 20
//This is the size of the time portion in the EEPROM
//Current format is time then depth for each syringe
#define SIZE_OF_TIME 4
//53 for MEGA, 10 for pro mini
#define CS_PIN 10

// Address locations
#define system_start_time_address 0
#define number_syringes_address 4
#define curr_syringe_address 6
#define syring_table_start_address 8
#define forward_flush_time_address 10
#define reverse_flush_time_address 12
#define pump_startup_time_address 14

// Pins
//If adding more pins make sure to update number of pins and the arrays below
#define syringePin1 2
#define syringePin2 3
#define syringePin3 4
#define syringePin4 5
#define syringePin5 6
#define syringePin6 7
#define syringePin7 8
#define syringePin8 9

int curr_syringe = 0;
int number_syringes = 0;
int syringe_table_start = 0;
//Pin definition
#define mosfestNumPins 4
const int mosfetPins[] = { syringePin1, syringePin2, syringePin3, syringePin4 };
#define selectNumPins 4
const int selectPins[] = { syringePin5, syringePin6, syringePin7, syringePin8 };

// Define Marco to make the Operation Mode and SetupMode compatible
// In Setupt mode the Serial print should always work
#define SerialPrintLN(stream) if( 1 == 1) { Serial.println(stream);}
#define SerialPrint(stream) if( 1 == 1) { Serial.print(stream);}


enum log_level {
  LOG_ERROR,
  LOG_WARNING,
  LOG_INFO,
  LOG_DEBUG,
};

enum module {
  DATA,
  SYSTEM,
  STATE,
};

//Temp variables for testing

/*
 * Function: setup()
 * 
 * Description: Sets up Serial Communction, and also sets up Perhiperals, sets up syringe sample data table to 
 *  EEPROM, and lastly resets configuration values to the start values
 */
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

  LogPrint(DATA, LOG_DEBUG, F("Setup Routine Done"));
  
}

void loop() {
  // nothing happens after setup finishes.
}

/*
 * Function: initSDcard()
 * 
 * Description: Sets up the SD card SPI interface. As a test, it creates and removed a file.
 */
void initSDcard()
{
  Serial.print(F("Initializing SD card..."));

  // CHange back to 10 for Pro mini
  if (!SD.begin(CS_PIN)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));

  //Opens a file, then tries to remove it, if it works then SD card init sucessful
  exampleFile = SD.open(exampleFileName, FILE_WRITE);
  if (SD.exists(exampleFileName)) 
  {   
    // Remove the file once the data is loaded
    SD.remove(exampleFileName);
    if (SD.exists(exampleFileName)) 
    {
      Serial.println(F("SD card init failed, couldn't remove file"));
    }    
    else{
      Serial.println(F("SD card init succesfully"));
    }
  } 
  else 
  {
    Serial.println(F("SD card init failed, couldn't open file"));
  }
  if(exampleFile)
  {
    exampleFile.close();
  }
}

/*
 * Function: initEEPROM()
 * 
 * Description: Set EEPROM values from the value in the config file from the SD card. If it cannot find the 
 *  config file, it will create it blank.
 */
void initEEPROM()
{
  // Open the configuration file  
    if (SD.exists(configFileName))
    {
      configFile = SD.open(configFileName, FILE_READ);
      if(configFile)
      {       
        int counter = 0;        
        char line[SIZE_OF_STRING];
        while(configFile.available()) 
        {     
          readLine(configFile,line, sizeof(line));
          String toString = String(line);
          //Value is long int because time_t is long int
          long int value = toString.toInt();
          String output = "";
          // Load system start time
          if (counter == 0)
          {
            EEPROM.put(system_start_time_address, (time_t)value);
            output = "Sys start time: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 1)
          {
            EEPROM.put(number_syringes_address, value);
            number_syringes = value;
            output = "# Syr: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 2)
          {
            EEPROM.put(curr_syringe_address, (int)value);
            output = "Curr Syringe : " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 3)
          {
            EEPROM.put(syring_table_start_address, (int)value);
            syringe_table_start = value;
            output = "table start: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 4)
          {            
            // Load reverse flush time
            EEPROM.put(forward_flush_time_address, (int)value);
            output = "For flush: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
            
          }
          else if (counter == 5)
          {
            //Load reverse flush time
            EEPROM.put(reverse_flush_time_address, (int)value);
            output = "Rev flush: " + (String)value;
            LogPrint(SYSTEM, LOG_INFO, output.c_str());
          }
          else if (counter == 6)
          {
            //Load reverse flush time
            EEPROM.put(pump_startup_time_address, (int)value);
            output = "Pump Start time: " + (String)value;
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
      configFile = SD.open(configFileName, FILE_READ);
      if(configFile)
      {
        configFile.close();
      }
      Serial.println(F("Could not find config file"));
      LogPrint(SYSTEM, LOG_ERROR, F("Could not find config file"));
    }
}

/*
 * Function: initRTC()
 * 
 * Description: Sets up RTC comms with I2C. Sets the RTC to the compile time of the sketch.
 */
void initRTC()
{
  // Initalizes RTC communcations
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (1);
  }
  //Since this is the setup routine, always set the RTC value
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.print(F("RTC value equals: "));
  String t ="";
  timestamp(t);
  Serial.println(t);
}

/*
 * Function: initSyringes()
 * 
 * Description: Sets all the syringe pins as outputs and make sure they are set low
 * 
 */
void initSyringes()
{
  for (int i =0; i< (mosfestNumPins + selectNumPins); i++)
  {
    //First set all the mosfet pins
    //Mosfet pins are the postive pin that selects the correct J component. (connected to PNP type Mosfet)
    if(i < mosfestNumPins)
    {
       SerialPrint(F("Mosfet: "));
       SerialPrintLN(mosfetPins[i]);
       pinMode(mosfetPins[i], OUTPUT);
       digitalWrite(mosfetPins[i], LOW);  
    }
    // make sure to set all the selector pins. Selector pins select which syringe inside
    // of each J componenent piece. (connected to NPN type mosfet
    else 
    {
       SerialPrint(F("selector: "));
       SerialPrintLN(selectPins[i-mosfestNumPins]);
       pinMode(selectPins[i-mosfestNumPins], OUTPUT);
       digitalWrite(selectPins[i-mosfestNumPins], LOW);  
    }
    
  }
  //Make sure the ports have time to settle
  LogPrint(SYSTEM, LOG_INFO, F("Done with init of syringe pins"));
  delay(500);
}

/*
 * Function: initPeripherals()
 * 
 * Description: Calls all the initalization functions 
 */
void initPeripherals()
{
  initSDcard();
  initRTC();  
  // Prevent actuationSyringes by accident
  initSyringes();
  initEEPROM();
}

/*
 * Function: syringeIteration()
 * 
 * Description: Loads the syringe sample data table to the correct spot in the EEPROM. Depending on if it loads
 *  the first 50 and second 50 is dependent on what the current syringe is. After testing time dif, without and
 *  log prints, it takes about 90 milliseconds to load 50 syringes to EEPROM
 */
void syringeIteration(){  
  LogPrint(SYSTEM, LOG_INFO, F("Starting syringeIteration"));  
  //unsigned long startTime = millis();
  if(SD.exists(sampleDataTableName))
  {
    sampleDataTableFile = SD.open(sampleDataTableName);
    if (sampleDataTableFile)
    { 
      // If the current syringe is greater than the number of syringes then all the syringes have been
      // Loaded so no need to increment the EEPROM memory
      if (curr_syringe >= number_syringes){
        LogPrint(SYSTEM, LOG_INFO, F("Quiting, curr_syringe > number_Syringes"));
        return; 
      }
      
      int start;
      int finish;
      // Determines whether it is in the first 50 or second 50 spot in EEPROM on where the 
      // the current syringe is
      // Functionality: When the syringe hits spot 0, it will populate it's current section of 50 syringes
      // AKA when current syringe hits 100, it will populate the EEPROM with data from 100-149
           
      if ( curr_syringe%100 < 50)
      {
        //first 50 spots in memory
         start = 0;
         finish = 49;
      }
      else
      {
        //last 50 spots in memory
         start = 50;
         finish = 99;
      }

      // Start iterating through until the line we want to start recording
      //the values from the file will be ignored
      int i=0;
      while (i<curr_syringe){
        long int a=0;
        int b=0;
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
        Serial.print(F("x: "));
        Serial.println(samTime);
        Serial.print(F("y: "));
        Serial.println(y);
        Serial.print(F("Address = :"));
        Serial.println(syringe_table_start + (i*SIZE_OF_SYRINGE));        
        Serial.println();

        // Put the csv values into the EEPROM
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE), samTime);
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE) + SIZE_OF_TIME, y);

        
      }
      sampleDataTableFile.close();
    }
    else
    {
      LogPrint(SYSTEM, LOG_ERROR, F("Could not open Sample Data File"));
    }
  }
  else
  {
    LogPrint(SYSTEM, LOG_ERROR, F("Sample Data table File does not exist"));
  }
  //unsigned long totTime = millis() - startTime;
  //Serial.println(totTime);
}

/*
 * Function: readline()
 * 
 * Description: Reads a line from the file and returns it if there is any content. 
 * 
 * Arguments:
 * &F -> The File passed by reference. This is the file that is opened to read from
 * line -> This is the pointer to black char[]. The line is passed back if there is anything
 * maxLen -> The max size to read from a single line
 */
bool readLine(File &f, char* line, size_t maxLen) {
  // reads in characters to the char* until end of file
  for (size_t n = 0; n < maxLen; n++) {
    // reads in a line from the file
    int c = f.read();
    // If it didn't find any data
    if ( c < 0 && n == 0) return false;  // EOF
    //when the null termintaor is hit, return the string 
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}

// This assumes the following format (example from table)
////////////////////////////////////////////////////
//1486567525,500
//1486787525,90
//1486600000,790
//1486567600,1054
//////////////////////////////////////////////////
// Thus the format follows
// long int, int
/*
 * Function: readVals()
 * 
 * Descriptoin: This function is specifically for reading an csv file. It looks for the format as descibed
 *  above. If it doesn't follow that format undesired consequenes may occur.
 *  
 * Arguments:
 * *v1 -> Size long int, since it corresponds to a time_t value. It stores in the first spot of a syringe memory layout
 * *v2 -> Size int, since it corresponds to a depth value. It stores in the second spot of a syringe memory layout
 */
bool readVals(long int* v1, int* v2) {
  char line[SIZE_OF_STRING], *ptr, *str;
  // reads a line from the SD card
  if (!readLine(sampleDataTableFile, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  // converts the string to a long int
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  // searchs for a comma
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  //converts part after , into an int
  *v2 = strtol(ptr, &str, 10);
  return str != ptr;  // true if number found
}



// Below is an example from the DATA log file
/*
  2017/2/9 12:19:49, Info, State 1 started
  2017/2/9 12:19:49, Info, Transitioning to State 2
  2017/2/9 12:19:49, Info, State 2 started
  2017/2/9 12:19:49, Info, Transitioning to state3
  2017/2/9 12:19:49, Info, State 3 started
  2017/2/9 12:19:49, Info, Transitioning to State 1
*/
/*
 * Function: LogPrint(module moduleName, log_level logLevel, const __FlashStringHelper* logData)
 * 
 * Description: Logging function that logs to a different file depending on what the log is for. It also
 *  prints the time and the level of log that each log is for.
 *  
 *  Arguments:
 *  ModuleName -> Expects one of the enumeration for module. This determines which file is used for logging
 *  logLevel -> Expects one of the enumeration for log_level. This determines the severity of the log and is printed with the logprint line
 *  const __FlashStringHelper* -> This is used so an F() macro can be passed to the function
 */
void LogPrint(module moduleName, log_level logLevel, const __FlashStringHelper* logData)
{
  File logFile;
  if (moduleName == DATA)
  {    
    logFile = SD.open(dataLogFile, FILE_WRITE);
  }
  else if (moduleName == SYSTEM)
  {
    logFile = SD.open(systemLogFile, FILE_WRITE);
  }
  else if( moduleName == STATE)
  {
    logFile = SD.open(stateLogFile, FILE_WRITE);
  }
  if (logFile)
  {
    String level;
    if (logLevel == LOG_ERROR)
    {
      level=F("Error");
    }
    else if (logLevel == LOG_WARNING)
    {
      level=F("Warning");
    }
    else if (logLevel == LOG_INFO)
    {
      level=F("Info");
    }
    else 
    {
      level=F("Debug");
    }
    
    // Switch to reading from the RTC once it is integrated
    String t = "";
    timestamp(t);
    String myStr = t + ", " + (String)level + ", "+ (String)logData;
    logFile.println(myStr);
    Serial.println(myStr);    
    logFile.close();
  }
}
/*
 * (NOTE) Same as above but takes in a char[] instead of a F()
 * Function: LogPrint(module moduleName, log_level logLevel, char logData[])
 * 
 * Description: Logging function that logs to a different file depending on what the log is for. It also
 *  prints the time and the level of log that each log is for.
 *  
 *  Arguments:
 *  ModuleName -> Expects one of the enumeration for module. This determines which file is used for logging
 *  logLevel -> Expects one of the enumeration for log_level. This determines the severity of the log and is printed with the logprint line
 *  logData[] -> This is the string that is passed into the logPrint
 */
void LogPrint(module moduleName, log_level logLevel, char logData[])
{
  File logFile;
  if (moduleName == DATA)
  {    
    logFile = SD.open(dataLogFile, FILE_WRITE);
  }
  else if (moduleName == SYSTEM)
  {
    logFile = SD.open(systemLogFile, FILE_WRITE);
  }
  else if( moduleName == STATE)
  {
    logFile = SD.open(stateLogFile, FILE_WRITE);
  }
  if (logFile)
  {
    String level;
    if (logLevel == LOG_ERROR)
    {
      level=F("Error");
    }
    else if (logLevel == LOG_WARNING)
    {
      level=F("Warning");
    }
    else if (logLevel == LOG_INFO)
    {
      level=F("Info");
    }
    else 
    {
      level=F("Debug");
    }
    
    // Switch to reading from the RTC once it is integrated
    String t = "";
    timestamp(t);
    String myStr = t + ", " + (String)level + ", "+ (String)logData;
    logFile.println(myStr);
    Serial.println(myStr);    
    logFile.close();
  }
}

/*
 * Function: timestamp(String &timeFormat) 
 * 
 * Descriptoin: Given a String based by reference, it returns the time stamp to that string
 * 
 * Arguments:
 * &timeFormat -> expects a String based by reference. The times stamp will be returned to this string
 */
void timestamp(String &timeFormat)
{
    DateTime now = rtc.now();
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}






