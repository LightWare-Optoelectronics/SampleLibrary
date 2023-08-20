#include <Wire.h> 
#include <LW_SF000.h> 
LW_SF000 sf000(Serial1);

void setup() {
  // setup code, to run once:
  
  Serial.begin(9600); 
  Serial.print("Serial initialized");
  Serial1.begin(115200);
  // Request the sf000 Hardware Name
  sf000.readRequestHardwareName();
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);

  // Disable any possible streaming data
  sf000.writeDataStreamType(0);
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);

  
  // Request the sf000 Hardware Name
  sf000.readRequestHardwareName();
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);

  // Request the sf000 Firmware Version
  sf000.readRequestFirmwareVersion();
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);

  // Request the streaming output distance data selection
  sf000.writeDistOutConfig(255);
  //sf000.readRequestDistOutConfig();
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);
  
  // Set the streaming to distance in cm
  sf000.writeDataStreamType(5);
  delay(100);
  // Read the data sent from the sf000
  sf000.ProcessSerialInput(1);
  delay(100);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  uint8_t new_data = 0; // Every cycle check the Serial recieve buffer for data and then process it
  new_data = sf000.ProcessSerialInput(1);
  // if new data was recieved, then display the following first and last distance
  if (new_data == 1){
    Serial.println(sf000.firstRaw_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.firstFiltered_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.firstStrength_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.lastRaw_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.lastFiltered_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.lastStrength_cm,DEC);
    Serial.print(" ");
    Serial.print(sf000.backgroundNoise,DEC);
    Serial.print(" ");
    Serial.print(sf000.APDTemperature,DEC);
    Serial.print(" ");
    Serial.print(sf000.FirmwareVersionMajor,DEC);
    Serial.print(" ");
    Serial.print(sf000.serialBaudrate,DEC);
    Serial.print(" \n");
    new_data = 0;
  }
  
  // put your main code here, to run repeatedly:
    
}