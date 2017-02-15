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
#include <time.h>
#include <EEPROM.h>

//53 for MEGA, 10 for pro mini
#define CS_PIN 53

// Set PC_COMMS to 1 if the PC is connected and should print to Serial
// Set PC_COMMS to 0 if the PC is not connected and it should not print to serial
#define PC_COMMS 1


//Global FIles
File sampleDataTableFile;
File exampleFile;

//File names
char sampleDataTableName[] = "sampleTB.csv";
char dataLogFile[] = "data.txt";
char systemLogFile[] = "system.txt";
char exampleFileName[] = "example.txt";

// Enum for different states
enum state {
    STATE1,
    STATE2,
    STATE3,
};

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

//Macros
#define SIZE_OF_SYRINGE 6
#define SIZE_OF_STRING 20
//This is the size of the time portion in the EEPROM
//Current format is time then depth for each syringe
#define SIZE_OF_TIME 4
// Address Macros
#define system_start_time_address 0
#define number_syringes_address 4
#define curr_syringe_address 6
#define syring_table_start_address 8
#define forward_flush_time_address 10

// Defines a fucntion below so that it doesn't use Serial.println if the comms are disabled
#define SerialPrintLN(stream) if( PC_COMMS == 1) { Serial.println(stream);}
#define SerialPrint(stream) if( PC_COMMS == 1) { Serial.print(stream);}

//Pins (Pin value changes based on board)
const int pressurePin = 7;
const int temperaturePin = 0; 

//Global Variables
time_t system_start = 0;
time_t curr_sample_time = 0;
int curr_syringe = 0;
int number_syringes = 0;
int syringe_table_start = 0;
state curr_state= STATE1;
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  SerialPrintLN(F("someString"));
  ///Set up peripherals + also variables
  initPeripherals(); 
  LogPrint(SYSTEM, LOG_INFO, "Starting Operation mode");
  
}

void loop() {

  DateTime now = rtc.now();
  time_t nowT = now.unixtime();
  //SerialPrintLN(nowT);
  //SerialPrintLN(system_start);
  //Wait to hit the system start time
  if(nowT > system_start)
  {
    //LogPrint(SYSTEM, LOG_INFO, "Start time hit");
    //SerialPrintLN("Start time hit");
    //mainLoop();
  }
  else 
  {    
    //LogPrint(SYSTEM, LOG_INFO, "Start not hit, sleeping");
    SerialPrintLN("Start not hit, sleeping");
    //Set timer for sleep
    //sleep(system_start - time.now)
  }
      
}

void mainLoop() {

  
  // set state to 1
  curr_state = STATE1;  
  int curr_pressure_level = 0;
  while(1) {
  
    // State 3 is first, so if it ever hits state 3 it hits it first and thus is priority
    // if state is 3
    if (curr_state == STATE3)
    {
        // Log that state 3 started
        LogPrint(STATE, LOG_INFO, "State 3 started");

        //Log Pressure
        int pressureValue = getVoltage(pressurePin);
        String output = "Press: " + (String)pressureValue;
        LogPrint(DATA, LOG_INFO, output.c_str());
        
        // start the sample sequence
        //startSampleSequence();
        
        // when sampling is done
        // Increment the curr syringe
        incrementSyringe();
        
        // Log setting state to 1
        LogPrint(STATE, LOG_INFO, "Transitioning to State 1");
        
        // set state back to 1
        curr_state = STATE1;
    }
  
    // if state is 2
    if(curr_state == STATE2)
    {
        // Log state 2 started
        LogPrint(STATE, LOG_INFO, "State 2 started");
        
        // read time from RTC to sync clock
        DateTime now = rtc.now();
        time_t nowT = now.unixtime();
        
        // if curr_time is not past sample time
        if (nowT < curr_sample_time)
        {
          // sleep until currTime
          // Sleep(until curr_sample_time);
          SerialPrintLN("In Here");
        }
        else
        {
          int pressureValue = 0;
          while(curr_state == STATE2)
          {            
            pressureValue = getVoltage(pressurePin);
            if (pressureValue == curr_pressure_level)
            {
              //Log Current Pressure
              String output = "Press: " + (String)pressureValue;
              LogPrint(DATA, LOG_INFO, output.c_str());

              // Log Stat change
              LogPrint(STATE, LOG_INFO, "Transitioning to state3");
              curr_state=STATE3;
            }
            else
            {
              //sleep for a couple of seconds
              LogPrint(SYSTEM, LOG_INFO, "Sleeping to poll the sensor again");
            }          
          }
        }
    }
  
    // if state is 1
    if(curr_state == STATE1)
    { 
        // IF the current syringe is passed the number of syringes then quit while loop and quit
        // Since curr_syringe is based 0 and num_sryinges is based 1, if they equal, also quit
        if( curr_syringe >= number_syringes)
        {
          break;
        }
        // Log state 1 started
        LogPrint(STATE, LOG_INFO, "State 1 started");
        
        // Sync Clock with RTC
        DateTime now = rtc.now();
        // set the pressure to check for and time to check for
        curr_pressure_level = currentSyringePressure();
        // Log setting state to 2
        LogPrint(STATE, LOG_INFO, "Transitioning to State 2");
        // Set state to 2
        curr_state = STATE2;
    }
  }
  LogPrint(SYSTEM, LOG_INFO, "All syringes incremented");
  while(1)
  {
    LogPrint(SYSTEM, LOG_INFO, "Sleeping forever");
    delay(10000);
  }
}

int currentSyringePressure()
{
  int pressureNow = 0;
  //Get the current syringe read in EEPROM the pressure 
  EEPROM.get(syringe_table_start + curr_syringe*SIZE_OF_SYRINGE+4, pressureNow);
  return pressureNow;
}

void incrementSyringe()
{
  //increment curr_syringe by 1
  curr_syringe++;
  //if curr_syringe == 0 or curr_syringe == 50
  if (curr_syringe%100 == 0 || curr_syringe%100 == 50)
  {
    LogPrint(SYSTEM, LOG_INFO, "Incrementing EEPROM");
    //then increment EEPROM 
    syringeIteration();
  }
  //else don't do anything
}

int getVoltage(int pin)
{
  return(analogRead(pin));
}


void initPeripherals()
{
  initSDcard();
  initRTC();  
  loadConfigVars();
}

void loadConfigVars()
{
  String output = "";
  
  //load system start from EEPROM
  EEPROM.get(system_start_time_address, system_start);
  output = "Sys start: " + (String)system_start;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);

  //Load num sryinges
  EEPROM.get(number_syringes_address, number_syringes);
  output = "# of syr: " + (String)number_syringes;  
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);
  
  //Load current syringe
  EEPROM.get(curr_syringe_address, curr_syringe);
  output = "Curr Syr: " + (String)curr_syringe;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);

  //Load syringe_table_start_address
  EEPROM.get(syring_table_start_address, syringe_table_start);
  output = "Table Start: " + (String)syringe_table_start;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);
}

void initRTC()
{
  if (! rtc.begin()) {
    SerialPrintLN(F("Couldn't find RTC"));
    LogPrint(SYSTEM, LOG_ERROR, "Could not find RTC");
    while (1);
  }
  //Since this is the operation mode, only adjust the RTC time if it was complied recently
  //The assumption is made that the Setup Mode was recently run before this so the RTC vary should not 
  //Be that much
  time_t compileTime = DateTime(F(__DATE__), F(__TIME__)).unixtime();
  DateTime now = rtc.now();
  time_t nowT = now.unixtime();
  long int timeDif = nowT - compileTime;
  // Get the absolute value
  timeDif = (timeDif * timeDif)/timeDif;
  // Reset the RTC to the compile time if the difference is less than 10 seconds
  // This prevents the RTC from reseting if the arudino shuts off and gets woken back up 
  if ( timeDif < 10)
  {
    SerialPrintLN(F("Adjusting"));
    LogPrint(SYSTEM, LOG_INFO, "Adj rtc");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  SerialPrint(F("RTC value equals: "));
  String t ="";
  timestamp(t);
  SerialPrintLN(t);
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
 * Function: syringeIteration()
 * 
 * Description: Loads the syringe sample data table to the correct spot in the EEPROM. Depending on if it loads
 *  the first 50 and second 50 is dependent on what the current syringe is. After testing time dif, without and
 *  log prints, it takes about 90 milliseconds to load 50 syringes to EEPROM
 */
void syringeIteration(){  
  LogPrint(SYSTEM, LOG_INFO, "Starting syringeIteration");  
  //unsigned long startTime = millis();
  if(SD.exists(sampleDataTableName))
  {
    sampleDataTableFile = SD.open(sampleDataTableName);
    if (sampleDataTableFile)
    { 
      // If the current syringe is greater than the number of syringes then all the syringes have been
      // Loaded so no need to increment the EEPROM memory
      if (curr_syringe >= number_syringes){
        LogPrint(SYSTEM, LOG_INFO, "Quiting, curr_syringe > number_Syringes");
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
      LogPrint(SYSTEM, LOG_ERROR, "Could not open Sample Data File");
    }
  }
  else
  {
    LogPrint(SYSTEM, LOG_ERROR, "Sample Data table File does not exist");
  }
  //unsigned long totTime = millis() - startTime;
  //Serial.println(totTime);
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


/*
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

// Below is an example from the DATA log file
/*
  2017/2/9 12:19:49, Info, State 1 started
  2017/2/9 12:19:49, Info, Transitioning to State 2
  2017/2/9 12:19:49, Info, State 2 started
  2017/2/9 12:19:49, Info, Transitioning to state3
  2017/2/9 12:19:49, Info, State 3 started
  2017/2/9 12:19:49, Info, Transitioning to State 1
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
  if (logFile)
  {
    String level;
    if (logLevel == LOG_ERROR)
    {
      level="Error";
    }
    else if (logLevel == LOG_WARNING)
    {
      level="Warning";
    }
    else if (logLevel == LOG_INFO)
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


/*
 * Function: timestamp(STring &timeFormat) 
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





