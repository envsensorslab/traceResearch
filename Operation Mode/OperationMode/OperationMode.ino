
///
// ENum for different states


void setup() {
  // put your setup code here, to run once:

  ///Set up peripherals
  // Put values from EEPROM to the SRAM
  // I.E System start time, flush times, current syring, num syringes, 

  

}

void loop() {
  // put your main code here, to run repeatedly:

  //Wait to hit the system start time
  // while not at system start time
  // delay for start_time - current time
  // after breaking loop
  // set state to 1


  //while past start time
  // 
  // if state is 1
  // set the pressure and time to check
  // set interrupt/when to trigger

  // if state is 2
  // read time from RTC to sync clock
  // if currTime is not past sampele time
  // sleep until currTime

  // else poll the pressure sensor
  // if the sensor is at the right value
  // set state to 3
  // else
  // DO a quick log, set time to sleep until it should start sampling


  // if state is 3
  // record the data
  // start the sample sequence
  // when sampling is done
  // Increment the curr syringe
  // set state back to 1

}
