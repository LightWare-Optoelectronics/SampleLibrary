/*
 * LightWare SF30 Analog Arduino Sample
 * http://www.lightware.co.za
 */
 
const int analogPin = 3;
const float sf30_analog_range = 32.0;
const float sf30_analog_voltage = 2.56;

void setup() {
  Serial.begin(115200);
}

void loop() {
  while (true)
  {
    float voltage = analogRead(analogPin) * (5.0 / 1024.0);
    float distance = (voltage / sf30_analog_voltage) * sf30_analog_range;
        
    Serial.print(distance, 4);
    Serial.println("m");
    delay(100);
  }
}
