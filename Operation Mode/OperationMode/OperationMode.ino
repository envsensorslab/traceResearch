
///
// Enum for different states
// enum state {
//    STATE1,
//    STATE2,
//    STATE3,
// }

//Global Variables
// time_t system_start = 0;
// int curr_syringe = 0;
// int num_syringes = 0;
// state curr_state= STATE1;

void setup() {
  // put your setup code here, to run once:

  ///Set up peripherals
  // Put values from EEPROM to the SRAM
  // I.E System start time, flush times, current syring, num syringes, 

  //load system start from EEPROM
  // system_start = EEPROM.READ();

  //Load current syringe
  // curr_syringe = EEPROM.READ()

  //Load num sryinges
  // num_sryinges = EEPROM.READ()

  

}

void loop() {

  
  //Wait to hit the system start time
  // If time.now > systemStart time
       // Log start main
       // LOGRPINT(...)
       // Start true main
       // main()
  // while not at system start time
  // else
      //LOG system time not reached
      //LOGPRINT(...)
      // set to sleep
      // sleep(system_start - time.now)
      
}

void main() {

  
  // set state to 1
  // curr_state = STATE1;
  while(1) {
  
  // State 3 is first, so if it ever hits state 3 it hits it first and thus is priority
  // if state is 3
      // Log that state 3 started
      // record the data
      // start the sample sequence
      // when sampling is done
      // Increment the curr syringe
      // Log setting state to 1
      // set state back to 1

  // if state is 2
      // Log state 2 started
      // read time from RTC to sync clock
      // if curr_time is not past sample time
      // sleep until currTime
  // else poll the pressure sensor
      //Loop until data is sampled
      // while(curr_state == STATE2)
          // if the sensor is at the right value (poll sensor)
              // Log state switching to 3
              // set state to 3
          // else
              // DO a quick log, set time to sleep until it should start sampling

  // if state is 1
      // Log state 1 started
      // Sync Clock with RTC
      // set the pressure to check for and time to check for
      // set interrupt/when to trigger
      // Log setting state to 2
      // Set state to 2
  }
}
