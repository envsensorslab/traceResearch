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
const char sampleDataTableName[] = "sampleTB.csv";



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //This method is used if we want to make it one program
  //initSDcard();
  
  numSryinges = setupRoutine();
  initPeripherals(numSryinges);
  
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

  // Case where file exists, it will initiate setup user mode
  if (SD.exists("example.txt")) {
    Serial.println("Loading new sample Data");
    setupRoutine();
    // Remove the file once the data is loaded
    SD.remove("example.txt");
  } 
  // Else it contiunues executing where it was before it was shut off
  else {
    Serial.println("Executing where the system left off");
  }
}

void initPeripherals(int counter)
{
  
}


void setupRoutine()
{
  
  int counter = 0;
  Serial.println(sampleDataTableName);
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
      //Store value in EEPROM

      // Only want to load 100 syringe data points, therefore break after that
      if (counter > 100)
      {
        break;
      }
    }
      sampleDataTableFile.close();
    } else {
      Serial.println("Error opening sampleFile.csv");
    }
  } else {
    Serial.println("File does not exist");
  }

  return counter;
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

