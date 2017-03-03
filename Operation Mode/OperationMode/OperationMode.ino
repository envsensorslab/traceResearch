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
#include <avr/pgmspace.h>
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif
// Note that F("THIS IS A STRING") notation stores strings in program memory.
// This prevents programs from running out of dynamic memory due to storage of strings.

//53 for MEGA, 10 for pro mini
#define CS_PIN 10

// Set PC_COMMS to 1 if the PC is connected and should print to Serial
// Set PC_COMMS to 0 if the PC is not connected and it should not print to serial
//!!!!!!!!!!!!!!!! Make sure to set to 1 when actually using the System!!!!!!!!!!!!!!!
//The serial prints cause a lot of slowdown especially with iteration, test system first
// with it set to 1 though
#define PC_COMMS 1


//Global FIles
File sampleDataTableFile;
File exampleFile;

//File names
char sampleDataTableName[] = "sampleTB.csv";
char dataLogFile[] = "data.csv";
char systemLogFile[] = "system.txt";
char exampleFileName[] = "example.txt";
char stateLogFile[] = "system.txt";

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
// Size of syringe is number of bytes
#define SIZE_OF_SYRINGE 6 
#define SIZE_OF_STRING 20
//This is the size of the time portion in the EEPROM
//Current format is time then depth for each syringe
#define SIZE_OF_TIME 4
#define SYRINGE_PIN_ACTUATION_DELAY 500 //in ms

// Address Macros
#define system_start_time_address 0
#define number_syringes_address 4
#define curr_syringe_address 6
#define syring_table_start_address 8
#define forward_flush_time_address 10
#define reverse_flush_time_address 12
#define pump_startup_time_address 14

// Defines a fucntion below so that it doesn't use Serial.println if the comms are disabled
#define SerialPrintLN(stream) if( PC_COMMS == 1) { Serial.println(stream);}
#define SerialPrint(stream) if( PC_COMMS == 1) { Serial.print(stream);}

//Pins (Pin value changes based on board)
#define pressurePin A7
#define temperaturePin A6
#define pumpDirection A2 //green LED
#define pumpEnable A3 //orange LED

//If adding more pins make sure to update number of pins and the arrays below
#define syringePin1 2
#define syringePin2 3
#define syringePin3 4
#define syringePin4 5
#define syringePin5 6
#define syringePin6 7
#define syringePin7 8
#define syringePin8 9

//Global Variables
time_t system_start = 0;
int curr_syringe = 0;
int number_syringes = 0;
int syringe_table_start = 0;
int forward_flush_time = 0;
int reverse_flush_time = 0;
int pump_start_time = 0;
state curr_state= STATE1;
RTC_DS3231 rtc;
boolean pumpOn = false;
//pumpForw == true -> forward direction
//pumpForw == false -> reverse direction
boolean pumpForw = true;

//Pin definition
// Realized in the Syringe Actuation function,
// Not fully modular yet, adding more Pins will break it,
#define mosfestNumPins 4
const int mosfetPins[] = { syringePin1, syringePin2, syringePin3, syringePin4 };
#define selectNumPins 4
const int selectPins[] = { syringePin5, syringePin6, syringePin7, syringePin8 };

//Pressure Calibration Constants
const float v_power = 9.75;
const float a = 0.066466;
const float b = 0.02523;
const float m = 0.199992;
const float b2 = 0.00386;


/*
 * Function: Setup()
 * 
 * Description: Sets up perhiperals for the operation mode
 */
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  ///Set up peripherals + also variables
  initPeripherals(); 
  LogPrint(SYSTEM, LOG_INFO, F("Starting Operation mode"));
  
}

/*
 * Function: loop()
 * 
 * Description: Loops until the start time is reached then goes into the true main program
 */
void loop() {

  DateTime now = rtc.now();
  time_t nowT = now.unixtime();
  SerialPrint(F("Time now: "));
  SerialPrintLN(nowT);
  SerialPrint(F("System start: "));
  SerialPrintLN(system_start);
  
  //Wait to hit the system start time
  if(nowT > system_start)
  {
    LogPrint(SYSTEM, LOG_INFO, F("Start time hit"));
    mainLoop();
  }
  else 
  {    
    SerialPrintLN(F("Start not hit, sleeping"));

    int long timeDif = system_start - nowT;
    sleepForTime(timeDif);
  }
      while (1);
}

/*
 * Function: mainLoop
 * 
 * Description: This is the true main loop that executes once the start time is reached
 *  This loop iterates through all the syringes and once all the syringes were sampled then 
 *  It quits and puts the arudino to sleep
 */
void mainLoop() {

  
  // set state to 1
  curr_state = STATE1;  
  int curr_pressure_level = 0;  
  time_t curr_sample_time = 0;
  
  while(1) {
  
    // State 3 is first, so if it ever hits state 3 it hits it first and thus is priority
    // if state is 3
    // Also, log data in DATA file now instead of in system
    if (curr_state == STATE3)
    {
        // Log that state 3 started
        LogPrint(STATE, LOG_INFO, F("State 3 started"));

        //Log Pressure
        int pressure_value = getVoltage(pressurePin);
        int temp_value = getVoltage(temperaturePin);
        String output = (String)pressure_value + ", " + (String)temp_value;
        String output2 = "Curr_syringe = " + (String)curr_syringe;
        LogPrint(SYSTEM, LOG_INFO, output2.c_str());
        LogPrint(DATA, LOG_INFO, output.c_str());
        
        // start the sample sequence
        sampleSequence();
        
        // when sampling is done
        // Increment the curr syringe
        incrementSyringe();
        
        // Log setting state to 1
        LogPrint(STATE, LOG_INFO, F("Transitioning to State 1"));
        
        // set state back to 1
        curr_state = STATE1;
    }
  
    // if state is 2
    if(curr_state == STATE2)
    {
        // Log state 2 started
        LogPrint(STATE, LOG_INFO, F("State 2 started"));
        
        // read time from RTC to sync clock
        DateTime now = rtc.now();
        time_t nowT = now.unixtime();
        Serial.println(nowT);
        Serial.println(curr_sample_time);
        
        // if curr_time is not past sample time
        if (nowT < curr_sample_time)
        {
          long int timeDif = curr_sample_time - nowT;          

          LogPrint(SYSTEM, LOG_INFO, "Sample time not hit, sleeping");
          sleepForTime(timeDif);
        }
        else
        {
          int pressureValue = 0;
          // Loops until the condition is meet, aka the pressure the desired pressure it hit
          while(curr_state == STATE2)
          {          
            pressureValue = getVoltage(pressurePin);
            if (pressureValue == curr_pressure_level)
            {
              //Log Current Pressure
              String output = "Press: " + (String)pressureValue;
              LogPrint(SYSTEM, LOG_INFO, output.c_str());

              // Log Stat change
              LogPrint(STATE, LOG_INFO, F("Transitioning to state3"));
              curr_state=STATE3;
            }
            else
            {
              //sleep for a couple of seconds
              LogPrint(SYSTEM, LOG_INFO, F("Sleeping to poll the sensor again"));
              SerialPrint(F("Looking for: "))
              SerialPrintLN(curr_pressure_level);    
              SerialPrint(F("Current Pressure: "));
              SerialPrintLN(pressureValue);               
              int temp_value = getVoltage(temperaturePin);
     
              delay(500); 
            }         
          }
        }
    }
  
    // if state is 1
    if(curr_state == STATE1)
    { 
        // IF the current syringe is passed the number of syringes then quit while loop and quit
        // Since curr_syringe is based 0 and num_sryinges is based 1, if they equal, also quit
        SerialPrintLN(curr_syringe);
        if( curr_syringe >= number_syringes)
        {
          LogPrint(SYSTEM, LOG_DEBUG, F("Quiting"));
          break;
        }
        // Log state 1 started
        LogPrint(STATE, LOG_INFO, F("State 1 started"));
        
        // set the pressure to check for and time to check for
        curr_pressure_level = currentSyringePressure();
        curr_sample_time = currentSyringeTime();
        SerialPrint(F("Current Pressure: "));
        SerialPrintLN(curr_pressure_level);
        
        // Log setting state to 2
        LogPrint(STATE, LOG_INFO, F("Transitioning to State 2"));
        // Set state to 2
        curr_state = STATE2;
    }
  }
  LogPrint(SYSTEM, LOG_INFO, F("All syringes incremented"));
  while(1)
  {
    LogPrint(SYSTEM, LOG_INFO, F("Sleeping forever"));
    delay(10000);
  }
}

/*
 * Function: sleepForTime(int long timeDif)
 * 
 * Description: Given a time difference it sleeps for an apporpriate amount of time to make sure that
 *  A clock skew does not miss the start time. It will also wake up to do a quick log. This function 
 *  should be incapsulated in a while loop so that it keeps getting called.
 *  
 *  Argument:
 *  int long -> This is the time difference between the time desired and now. It should be performed by
 *    comparing unixtimes in seconds.
 */
void sleepForTime(int long timeDif)
{
   if (timeDif > 540)
    {
      LogPrint(SYSTEM, LOG_INFO, F("Sleeping for 6 minutes"));
      delay(360000);
    }
    // If the time is more than a 2.5 min but less than 8, then sleep for 2 minutes increments
    else if (timeDif > 150)
    {      
      LogPrint(SYSTEM, LOG_INFO, F("Sleeping for 2 minutes"));
      delay(120000);
    }
    else if (timeDif > 45)
    {      
      LogPrint(SYSTEM, LOG_INFO, F("Sleeping for 30 second"));
      delay(30000);
    }
    else if (timeDif > 5)
    {
      LogPrint(SYSTEM, LOG_INFO, F("Sleeping for 1 second"));
      delay(1000);
    }
    // When the time gets close, just sleep for half a second incremements
    else if (timeDif > 1)
    {
      LogPrint(SYSTEM, LOG_INFO, F("Sleeping for half a second"));
      delay(500);
    }
    // After it gets to 1 second, then just loop with out stopping
}

/* 
 *  Function currentSyringePressure()
 *  
 *  Description: Reads the pressue that corresponds with the current syringe
 *  
 *  Return:
 *  Returns the pressue 
 */
int currentSyringePressure()
{
  int pressureNow = 0;
  //Get the current syringe read in EEPROM the pressure 
  EEPROM.get(syringe_table_start + (curr_syringe%100)*SIZE_OF_SYRINGE+SIZE_OF_TIME, pressureNow);
  return pressureNow;
}

/*
 * Function: currentSyringeTime()
 * 
 * Description: Returns the start time of the current syringe
 * 
 * Returns:
 * time_t -> Returns the start time of the current syringe
 */
time_t currentSyringeTime()
{
  time_t time_get = 0;
  EEPROM.get(syringe_table_start + (curr_syringe%100)*SIZE_OF_SYRINGE, time_get);
  return time_get;
}

/*
 * Function: incrementSyringe()
 * 
 * Description: increments the current syringe and when necessary it executes a syringe iteration
 */
void incrementSyringe()
{
  LogPrint(SYSTEM, LOG_DEBUG, "Start incrementSyringe");
  //increment curr_syringe by 1
  SerialPrintLN(curr_syringe);
  curr_syringe++;
  writeCurrSyringeToEEPROM(curr_syringe);
  SerialPrint(F("Current Syringe: "));
  SerialPrintLN(curr_syringe);
  if (curr_syringe%100 == 0 || curr_syringe%100 == 50)
  {
    LogPrint(SYSTEM, LOG_INFO, F("Incrementing EEPROM"));
    //then increment EEPROM 
    syringeIteration();
  }
}

/*
 * Function: writeCurrSyringeToEEPROM(int syringe)
 * 
 * Description: Given the current syringe value it writes it to EEPROM
 * 
 * Arugment:
 * int -> The current syringe value which will be written to the EEPROM
 */
void writeCurrSyringeToEEPROM(int syringe)
{
  EEPROM.write(curr_syringe_address, syringe);
}

/*
 * Function:getVoltage(int pin)
 * 
 * Description: Given a pin, return the analog value of that pin
 * 
 * Argument:
 * int -> The pin number that should be read from
 * 
 * Return:
 * The analog value of that pin
 */
int getVoltage(int pin)
{
  return(analogRead(pin));
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
  initSyringes();
  initPump();
  loadConfigVars();
}

/*
 * Function: initSyringes()
 * 
 * Description: Sets all the syringe pins as outputs and make sure they are set low
 * 
 */
void initSyringes()
{
  LogPrint(SYSTEM, LOG_INFO, F("Start initSyringes"));
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
 * Function: loadConfigVars()
 * 
 * Description: Loads all the values from EEPROM to the SRAM (Global Variables)
 * 
 */
void loadConfigVars()
{
  LogPrint(SYSTEM, LOG_INFO, F("Start loadConfigVars"));
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

  //Load pump forward time
  EEPROM.get(forward_flush_time_address,forward_flush_time);
  output = "For flush: " + (String)forward_flush_time;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);

  //Load pump reverse time
  EEPROM.get(reverse_flush_time_address, reverse_flush_time);
  output = "Rev flush: " + (String)reverse_flush_time;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);

  //Load pump startup time
  EEPROM.get(pump_startup_time_address, pump_start_time);
  output = "Pump Start Time: " + (String)reverse_flush_time;
  LogPrint(SYSTEM, LOG_INFO, output.c_str());
  SerialPrintLN(output);
}

/*
 * Function: initRTC()
 * 
 * Description: Sets up RTC comms with I2C. Sets the RTC to the compile time of the sketch.
 *  Also, this checks the compile time and only sets it if it is within 10 seconds, otherwise the compile time
 *  Was a long time ago and the arudino just reset
 */
void initRTC()
{
  if (! rtc.begin()) {
    SerialPrintLN(F("Couldn't find RTC"));
    LogPrint(SYSTEM, LOG_ERROR, F("Could not find RTC"));
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
   if ( timeDif < 20)
   {
     SerialPrintLN(F("Adjusting"));
     LogPrint(SYSTEM, LOG_INFO, F("Adj rtc"));
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0,0,0,12));
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
  SerialPrintLN(F("Initializing SD card..."));

  // CHange back to 10 for Pro mini
  if (!SD.begin(CS_PIN)) {
    SerialPrintLN(F("initialization failed!"));
    LogPrint(SYSTEM, LOG_ERROR, F("SD card initialization failed!"));
    while (1);
  }
  SerialPrintLN(F("initialization done."));

  //Opens a file, then tries to remove it, if it works then SD card init sucessful
  exampleFile = SD.open(exampleFileName, FILE_WRITE);
  if (SD.exists(exampleFileName)) 
  {   
    // Remove the file once the data is loaded
    SD.remove(exampleFileName);
    if (SD.exists(exampleFileName)) 
    {
      SerialPrintLN(F("SD card init failed, couldn't remove file"));
    }    
    else{
      SerialPrintLN(F("SD card init succesfully"));
    }
  } 
  else 
  {
    SerialPrintLN(F("SD card init failed, couldn't open file"));
  }
  if(exampleFile)
  {
    exampleFile.close();
  }
}

/*
 * Function: initPump()
 * 
 * Description: Intalizes pump pins to output
 */
void initPump()
{
  pinMode(pumpDirection, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
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
      // Since the first 100 syringes are loaded with the setup routine, don't worry about the iteration
      // Until the syringe hits 100
      if(curr_syringe < 99)
      {
        LogPrint(SYSTEM, LOG_INFO, F("Still, below init"));
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
        
        SerialPrintLN(i);        
        SerialPrint(F("x: "));
        SerialPrintLN(samTime);
        SerialPrint(F("y: "));
        SerialPrintLN(y);
        SerialPrint(F("Converted pressure: "));
        SerialPrintLN(meters_to_mV(y));
        SerialPrint(F("Address = :"));
        SerialPrintLN(syringe_table_start + (i*SIZE_OF_SYRINGE));        
        SerialPrintLN();

        // Put the csv values into the EEPROM
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE), samTime);
        EEPROM.put(syringe_table_start + (i*SIZE_OF_SYRINGE) + SIZE_OF_TIME, meters_to_mV(y));

        
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
  //SerialPrintLN(totTime);
}

/*
 * Function: syringe_actuation()
 * 
 * Description: Sets the corresponding syringe pin to high to all for sampling
 * 
 * Reliance:
 * Relies on the global variable curr_syringe.
 */
void syringeActuation()
{
  int pinHigh = 0;
  int pinLow = 0;

  // Need to use the Marco to divide by 4 to make it fully modular.
  // Test before changin
  SerialPrintLN(curr_syringe/4);
  pinHigh = mosfetPins[(int)curr_syringe/4];

  SerialPrintLN(curr_syringe%(mosfestNumPins));
  pinLow = selectPins[curr_syringe%(selectNumPins)];
 
  SerialPrint(F("pinHigh is "));
  SerialPrintLN(pinHigh);
  SerialPrint(F("PinLow is "));
  SerialPrintLN(pinLow);
  digitalWrite(pinHigh, HIGH);
  digitalWrite(pinLow, HIGH);
  //Need to figure out the delay for actuation the syringe
  delay(SYRINGE_PIN_ACTUATION_DELAY);
  digitalWrite(pinHigh, LOW);
  digitalWrite(pinLow, LOW);  
}

/*
 * Function: sampleSequence()
 * 
 * Descrition: Initiates a sample sequence which takes the data measurements and starts pumping
 */
void sampleSequence()
{
  pumpSequenceForward();  
  syringeActuation();
  // Start pumping in the reverse direction
  pumpSequenceBackward();
}

/*
 * Function: pump_sequence_forware()
 * 
 * Description: Starts the pump in the forward direction for the specified amount of time
 *  from the EEPROM
 *  
 *  Relies:
 *    forward_flush_time: which is a global variable read in from the EEPROM
 */
void pumpSequenceForward(){
  LogPrint(SYSTEM, LOG_INFO, F("Start pumpSequenceForware"));
  SerialPrintLN(F("Start forward sequence"));

  pumpForw = true;
  pumpOn = true;  
  updatePumpState(pumpOn,pumpForw);
  SerialPrintLN(F("starting delay"));
  delay(pump_start_time);
  delay(forward_flush_time);
  pumpOn= false;
  updatePumpState(pumpOn, pumpForw);
}

/*
 * Function: pump_sequence_backward()
 * 
 * Description: Starts the pum in the reverse direction for the specified amount of time
 * 
 * Relies:
 *  reverse_flush_time: which is a global variable read in from the EEPROM
 */
void pumpSequenceBackward(){
  LogPrint(SYSTEM, LOG_DEBUG, F("Start pumpSequnceBackward"));
  //when timer hits value, reverse pin direction
  //set direction pin to low  
  //set pump enable to low
  SerialPrintLN(F("Starting backup function"));
  pumpForw = false;
  pumpOn = true;
  updatePumpState(pumpOn, pumpForw);
  delay(pump_start_time);
  delay(reverse_flush_time);
  pumpOn= false;
  updatePumpState(pumpOn, pumpForw);
}

/*
 * Function: updatePumpState(boolean pumpPower, boolean pumpDLR)
 * 
 * Description: Updates the current state of the pump. It is passed in the direction the pump should be 
 *  and also if the pump should be enabled. 
 *  
 *  Arguments:
 *    boolean pumpPower: True or False to indicate whether the pump enable should occur
 *    boolean pumpDLR: True or False to indicate the direction the pump should pump
 *      True -> Forward direction
 *      False -> Reverse direction
 *      
 *  Relies:
 *    The pin numbers for the pumpDirection and the pumpEnable
 */
void updatePumpState(boolean pumpPower, boolean pumpDLR){
  LogPrint(SYSTEM, LOG_DEBUG, F("Start updatePumpState"));
  SerialPrintLN(F("updating pump state"));
  pumpForw = pumpDLR;
  digitalWrite(pumpDirection, pumpForw);
  pumpOn = pumpPower;
  digitalWrite(pumpEnable, pumpOn);
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

/*
 * Function: meter_to_mV(float meters)
 * 
 * Description: Given the value in meters it converts it to the arudino analog read value
 * 
 * Arguments:
 * float meters -> The meter value that needs to be converted to the arudino analog read
 * 
 * Returns:
 * float -> The Pressure associated with the meter value
 * 
 */
int meters_to_mV(float meters){
  LogPrint(SYSTEM, LOG_DEBUG, F("Start meters_to_mV"));
  //reverse of the above subfunction
  //given depth by client
  float pressure;
  float p_arduino;
  float pv_arduino;
  float pv_sensor;


  pressure = ((meters * 14.57/10) + 14.7);
  pv_sensor = (v_power/10)*(pressure*m + b2);
  pv_arduino = pv_sensor*a + b;
  p_arduino = (pv_arduino*1023)/5;

  
  SerialPrint(F("Depth: "));
  SerialPrintLN(meters);
  SerialPrint(F("Pressure: "));
  SerialPrintLN(pressure);
  SerialPrint(F("PV_sensor:  "));
  SerialPrintLN(pv_sensor);
  SerialPrint(F("PV_arduino: "));
  SerialPrintLN(pv_arduino);
  SerialPrint(F("P_arduino: "));
  SerialPrintLN(p_arduino);

  return p_arduino + .5;
 
} //closes function

// This assumes the following format (example from table)
////////////////////////////////////////////////////
//1486567525,500
//1486787525,90
//1486600000,790
//1486567600,1054
//////////////////////////////////////////////////
// Thus the format follows
// long int, int -> as csv file
// Remember the start time must be in unixtime of the timezone that the user compiled the code
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
    SerialPrintLN(myStr);    
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
    //logFile = SD.open(stateLogFile, FILE_WRITE);
    //For now log state events in system log because it may be useful together
    logFile = SD.open(systemLogFile, FILE_WRITE);
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
    SerialPrintLN(myStr);    
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





