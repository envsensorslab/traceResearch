#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <time.h>
#include <EEPROM.h>

// Enum for different states
enum state {
    STATE1,
    STATE2,
    STATE3,
};

enum error_level {
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
#define system_start_time_address 0
#define number_syringes_address 4
#define curr_syringe_address 6
#define syring_table_start_address 8
#define forward_flush_time_address 10

//Global Variables
time_t system_start = 0;
time_t curr_sample_time = 0;
int curr_pressure_level = 0;
int curr_syringe = 0;
int num_syringes = 0;
state curr_state= STATE1;
RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // put your setup code here, to run once:
  Serial.println("hi");
  ///Set up peripherals + also variables
  initPeripherals(); 
}

void loop() {

  DateTime now = rtc.now();
  time_t nowT = now.unixtime();
  Serial.println(nowT);
  Serial.println(system_start);
  //Wait to hit the system start time
  if(nowT > system_start)
  {
    // Log start main
       // LOGRPINT(...)
       // Start true main
       // mainLoop()
    //LogPrint(SYSTEM, LOG_INFO, "Start time hit");
    Serial.println("Start time hit");
    //mainLoop();
  }
  else 
  {    
    // while not at system start time
    // else
      //LOG system time not reached
      //LOGPRINT(...)
      // set to sleep
      // sleep(system_start - time.now)
    //LogPrint(SYSTEM, LOG_INFO, "Start not hit, sleeping");
    Serial.println("Start not hit, sleeping");
    //Set timer for sleep
       
  }
      
}

void mainLoop() {

  
  // set state to 1
  // curr_state = STATE1;
  while(1) {
  
    // State 3 is first, so if it ever hits state 3 it hits it first and thus is priority
    // if state is 3
    if (curr_state == STATE3)
    {
        // Log that state 3 started
        LogPrint(STATE, LOG_INFO, "State 3 started");
        // record the data
        // start the sample sequence
        //startSampleSequence();
        // void startSampleSequence()
        // when sampling is done
        // Increment the curr syringe
        //incrementSyringe();
        // void incrementSyringe()
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
        if (nowT > curr_sample_time)
        {
          // sleep until currTime
          // Sleep(until curr_sample_time);
        }
        else
        {
          int pressureValue;
          //pressureValue = getSensorValues();
          while(curr_state == STATE2)
          {
            if (pressureValue == curr_pressure_level)
            {
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
        // Log state 1 started
        LogPrint(STATE, LOG_INFO, "State 1 started");
        // Sync Clock with RTC
        DateTime now = rtc.now();
        // Set arudino clock equal to rtc clock
        // set the pressure to check for and time to check for
        //curr_pressure_level = currgetSyringePressue();
        // Log setting state to 2
        LogPrint(STATE, LOG_INFO, "Transitioning to State 2");
        // Set state to 2
        curr_state = STATE2;
    }
  }
}

void initPeripherals()
{
  Serial.println("Starting");
  initSDcard();
  initRTC();  
  loadConfigVars();
}

void loadConfigVars()
{
  String output = "";
  
  //load system start from EEPROM
  EEPROM.get(system_start_time_address, system_start);
  output = "System start time is: " + (String)system_start;
  Serial.println(output);

  //Load current syringe
  EEPROM.get(curr_syringe_address, curr_syringe);
  output = "Current Syringe: " + (String)curr_syringe;
  Serial.println(output);

  //Load num sryinges
  EEPROM.get(number_syringes_address, num_syringes);
  output = "Number of syringes: " + (String)num_syringes;
  Serial.println(output);
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
  else if (moduleName == STATE)
  {
    logFile = SD.open("state.txt", FILE_WRITE);
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
    Serial.println(now.unixtime());
    
    timeFormat = (String)now.year() + "/" + (String)now.month() + "/" + (String)now.day() + " " + 
    (String)now.hour() + ":" + (String)now.minute() + ":" + (String)now.second();
  
}


