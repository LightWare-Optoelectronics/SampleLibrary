//---------------------------------------------------------
// LightWare SF30 Arduino Serial Sample
//---------------------------------------------------------

// This sample uses a hardware serial port on the Arduino.

#define SF30_SERIAL Serial1

void setup() {
  Serial.begin(57600);

  // SF30 configured to 400Hz and 19200bps.
  SF30_SERIAL.begin(19200);
}

float takeReading()
{
  // Flush buffer
  while (SF30_SERIAL.available())
    SF30_SERIAL.read();
  
  while (SF30_SERIAL.available() == 0);
  unsigned long t0 = micros();
  while (SF30_SERIAL.available() == 1);
  unsigned long t1 = micros();
  while (SF30_SERIAL.available() == 2);
  unsigned long t2 = micros();

  int byte0 = SF30_SERIAL.read();
  int byte1 = SF30_SERIAL.read();
  int byte2 = SF30_SERIAL.read();

  float distance = 0.0;

  if (t1 - t0 < t2 - t1)
    distance = byte1 + byte0 / 256.0;
  else
    distance = byte2 + byte1 / 256.0;

  return distance;
}

void loop() {
  // A reading can be taken at any time, so other work can be done before/after.
  float reading = takeReading();

  // Output results to console.
  Serial.print(reading);
  Serial.println("m ");
}