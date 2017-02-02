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

File root;
File myFile;
File sampleDataTableFile;
const char sampleDataTableName[] = "sampleTB.csv";



void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  if (SD.exists("example.txt")) {
    Serial.println("Loading new sample Data");
    setupRoutine();
  } else {
    Serial.println("Executing where the system left off");
  }
  SD.remove("example.txt");
  if (SD.exists("example.txt")) {
    Serial.println("Loading new sample Data");
  } else {
    Serial.println("Executing where the system left off");
  }
  myFile=SD.open("example.txt", FILE_WRITE);
  myFile.close();
  if (SD.exists("example.txt")) {
    Serial.println("Loading new sample Data");
  } else {
    Serial.println("Executing where the system left off");
  }
  setTime(now());
  TimeElements tm;
  tm.Second = 0;
  tm.Minute = 20;
  tm.Hour = 0;
  tm.Wday = 0;
  tm.Day = 5;
  tm.Month=3;
  tm.Year = 47;
  time_t t = makeTime(tm);
  Serial.println(second(t));
  Serial.println(minute(t));
  Serial.println(day(t));
  Serial.println(weekday(t));
  Serial.println(month(t));
  Serial.println(year(t));
  Serial.println(t);

  Serial.println("starting to print next date");
  time_t t2 = 1488780001;
  
  Serial.println(second(t2));
  Serial.println(minute(t2));
  Serial.println(day(t2));
  Serial.println(weekday(t2));
  Serial.println(month(t2));
  Serial.println(year(t2));
  Serial.println(t2);

}

void loop() {
  // nothing happens after setup finishes.
}

void setupRoutine()
{
  Serial.println(sampleDataTableName);
  if(SD.exists(sampleDataTableName))
  {
    sampleDataTableFile = SD.open(sampleDataTableName);
    if (sampleDataTableFile)
    {
      
    int x,y;
      while (readVals(&x, &y)) {
        Serial.print("x: ");
        Serial.println(x);
        Serial.print("y: ");
        Serial.println(y);
        Serial.println();
      }
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

