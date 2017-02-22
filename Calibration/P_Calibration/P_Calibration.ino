//The purpose of this subfunction is to calibrate the pressure sensor 

const int pressurePin = A7;    // pressure sensor 

// variables:
int sensorValue = 0;         // sensor value
int sensorMin = 1023;        // min sensor value
int sensorMax = 0;           // max sensor value


void setup() {
  // calibrate during the first five seconds
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }

}

void loop() {
  // read the sensor:
  sensorValue = analogRead(sensorPin);

  // apply the calibration to the sensor reading
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 1023);

  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, 0, 1023);

}
