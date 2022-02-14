// www.lightware.co.za          March 2021
// Serial and I2C interface for the SF000/D
// Modified by Andrw Yang (@druyang)
//----------------------------------------

#include <inttypes.h>
#include <Wire.h>

#include "lw_SF000.h"


LW_SF000::LW_SF000(HardwareSerial &serial1){
	Serial1 = &serial1;
}


void LW_SF000::test(){
	Serial.print("Print Test ");
	Serial.println("Print Test ");
	Serial.println("Print Test ");

}


//----------------------------------------
//------- Serial Interface ---------------
//----------------------------------------

// The serial communication works on the basis that the SF000/D will only
// reply with data when asked, except in the case where streaming was
// enabled where it will then continuosely send data.

// Serial communication Packet for Write and Read
    // Serial communication header
    // Byte:0 - Start Byte (always 0xAA)
    // Byte:1 - Flags Low Byte
    // Byte:2 - Flags High Byte
                // Flags Bits 6-15 is number of Data bytes (1 to 1023)
                // Flags Bits 1 to 5 reserved
                // Flags Bit 0 indicated Write(1) or Read(0)
    // Serial Communication Body (n = number of data bytes)
    // Byte:3 - Packet ID
    // Byte:4 up to Byte:4+n - Data:0 to Data:n

    // Serial Communication Checksum
    // Byte:n+5 - Checksum CRC Low Byte
    // Byte:n+6 - Checksum CRC High Byte

 // Packet ID descriptions - Typical operational IDs (Refer to documentation for full list)
 // 0 - Product Name (Read Flag bit will request data from SF000/D, than the SF000/D reply)
 // 1 - Hardware Version (Read Flag bit will request data from SF000/D, than the SF000/D reply)
 // 2 - Frimware Version (Read Flag bit will request data from SF000/D, than the SF000/D reply)
 // 3 - Serial Number (Read Flag bit will request data from SF000/D, than the SF000/D reply)
 // 10 - Token (Read Flag bit will request data from SF000/D, than the SF000/D reply)
 // 12 - Save Parameters (Write flag bit must be set)
 // 14 - Reset (Write flag bit must be set)
 // 27 - Distance output configuration (Read Flag bit will request current setting from SF000/D, then the SF000/D reply)
 //                                    (Write flag bit with data will change current setting on SF000/D, then the SF000/D reply)
 // 30 - Current Data Stream Type (Read Flag bit will request current setting from SF000/D, then the SF000/D reply)
 //                               (Write flag bit with data will change current setting on SF000/D, then the SF000/D reply)
 // 79 - Serial BaudRate (Read Flag bit will request current setting from SF000/D, then the SF000/D reply)
 //                      (Write flag bit with data will change current setting on SF000/D, then the SF000/D reply)
 // 44 - Distance Data (Read Flag bit will request current data from SF000/D, then the SF000/D reply)
 // 80 - I2C address (Read Flag bit will request current setting from SF000/D, then the SF000/D reply)
 //                  (Write flag bit with data will change current setting on SF000/D, then the SF000/D reply)



// This function creates the CRC for the Serial data
uint16_t LW_SF000::createCRC(uint8_t* Data, uint16_t Size)
{
    uint16_t crc = 0;
    for (uint32_t i = 0; i < Size; ++i)
    {
        uint16_t code = crc >> 8;
        code ^= Data[i];
        code ^= code >> 4;
        crc = crc << 8;
        crc ^= code;
        code = code << 5;
        crc ^= code;
        code = code << 7;
        crc ^= code;
    }
    return crc;
}

// This will request the product Hardware Name
void LW_SF000::readRequestHardwareName(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 0;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the product Hardware Version Number
void LW_SF000::readRequestHardwareVersion(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 1;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the product Firmware Version number
void LW_SF000::readRequestFirmwareVersion(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 2;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the product Serial Number
void LW_SF000::readRequestSerialNumber(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 3;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the next usable Token
void LW_SF000::readRequestToken(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 10;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the current Distance Output Configuration settings
void LW_SF000::readRequestDistOutConfig(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 27;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the current Data Stream Type setting
void LW_SF000::readRequestDataStreamType(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 30;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the current Serial interface Baudrate
void LW_SF000::readRequestSerialBaudrate(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 79;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the current Distance Output as per the Distance output Configuration settings
void LW_SF000::readRequestDistance(void){
  uint16_t CRC = 0;

  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 44;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  Serial.println(serialOutputBuffer[1]);

  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the current I2C Address
void LW_SF000::readRequestI2CAddress(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 80;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the APD Temperature
void LW_SF000::readRequestAPDTemp(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 57;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will request the Sampling Rate
void LW_SF000::readRequestSamplingRate(void){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 66;
  serialOutputHeader.Length = 1;
  serialOutputHeader.Write_Read = 0;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 4);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[4],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],6);
}

// This will send a save command.
// A new token must be requested and recieved and then passed to this
// function to send with the command
void LW_SF000::writeSaveParameters(uint16_t Token){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 12;
  serialOutputHeader.Length = 3;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Token,2);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 6);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[6],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],8);
}

// This will send a Reset command.
// A new token must be requested and recieved and then passed to this
// function to send with the command
void LW_SF000::writeResetCommand(uint16_t Token){
  uint16_t CRC = 0;
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 14;
  serialOutputHeader.Length = 3;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Token,2);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 6);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[6],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],8);
}

// This will change the Distance Output configuration
// The integer value of the selected bits must be passed to the function, alternative pass a 0 value
// and then the configuration will be set in the function below
void LW_SF000::writeDistOutConfig(uint32_t Value){
  uint16_t CRC = 0;

  //When Value is set to 0, the selected display outputs is set below,
  //otherwise when Value is not 0, it will dictate which outputs.
  //The following bits must be set to output value:
  //bit0 - First Return Raw
  //bit1 - First Return Filtered
  //bit2 - First Return Strength
  //bit3 - Last Return Raw
  //bit4 - Last Return Filtered
  //bit5 - Last Return Strength
  //bit6 - Background Noise
  //bit7 - Temperature

  // if no setting was passed to the function, we use the following:
  if (Value == 0){
    // comment ones not the be displayed
    Value |= 0x01;  //bit0 - First Return Raw
    //Value |= 0x02;  //bit1 - First Return Filtered
    Value |= 0x04;  //bit2 - First Return Strength
    Value |= 0x08;  //bit3 - Last Return Raw
    //Value |= 0x10;  //bit4 - Last Return Filtered
    Value |= 0x20;  //bit5 - Last Return Strength
    Value |= 0x40;  //bit6 - Background Noise
    Value |= 0x80;  //bit7 - Temperature
  }
  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 27;
  serialOutputHeader.Length = 5;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Value,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 8);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[8],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],10);

  DistanceOutConfig = Value;
}

// This will change the Data Stream output Type
// The integer value of the selected Typt must be passed to the function
// The available stream types are:
// 0 - Disable Streaming
// 10 - Signal Probability Data (reply with ID 200 and 201)
// 5 - Distance Data in cm(reply with ID 44)(output as set with Distance Output Configuration ID 29)
void LW_SF000::writeDataStreamType(uint32_t Type){
  uint16_t CRC = 0;

  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 30;
  serialOutputHeader.Length = 5;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Type,4);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 8);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[8],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],10);
}

// This will change the Serial Baudrate
void LW_SF000::writeSerialBaudrate(uint8_t Rate){
  uint16_t CRC = 0;

  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 79;
  serialOutputHeader.Length = 2;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Rate,1);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 5);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[5],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],7);
}

// This will change the I2C interface Address
void LW_SF000::writeI2CAddress(uint8_t Address){
  uint16_t CRC = 0;

  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 80;
  serialOutputHeader.Length = 2;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Address,1);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 5);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[5],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],7);
}

// This will change the Sampling Rate
void LW_SF000::writeSamplingRate(uint8_t Rate){
  uint16_t CRC = 0;

  // Setup the Header Data
  serialOutputHeader.Start = 0xAA;
  serialOutputHeader.ID = 66;
  serialOutputHeader.Length = 2;
  serialOutputHeader.Write_Read = 1;

  // move the header to the output buffer
  memcpy(&serialOutputBuffer[0],&serialOutputHeader,4);
  // move the data to the output buffer
  memcpy(&serialOutputBuffer[4],&Rate,1);
  // calculate the CRC
  CRC = createCRC(&serialOutputBuffer[0], 5);
  // move the CRC to the output buffer
  memcpy(&serialOutputBuffer[5],&CRC,2);
  // write the output buffer to the serial port
  Serial1->write(&serialOutputBuffer[0],7);
}

// Read data sent back from the SF000/D
// This function will check if new input data is available and will then
// process it.
// Data received will then be unpacked and saved in global variables.
// The function will return 1 if a new data packet has been processed.
// Pass 1 to the function if you want to report the received data on the
// serial terminal
uint8_t LW_SF000::ProcessSerialInput(int16_t report){
  uint8_t temp_byte = 0;
  uint16_t calc_crc = 0;
  uint16_t recv_crc = 0;
  uint8_t new_data = 0;
  uint32_t tempSel = DistanceOutConfig;
  uint32_t data_cnt = 0;
  uint8_t raw_stream_cnt = 0;
  int16_t i = 0;
  //Need to check if there is any new data available in the receive buffer
  while (Serial1->available()) {
    if (serialInputCount == 0){

      temp_byte = Serial1->read();

      if (temp_byte == 0xAA){
        serialInputBuffer[serialInputCount] = temp_byte;
        serialInputCount++;
      }
    }
    else {
      serialInputBuffer[serialInputCount] = Serial1->read();
      //Serial.println(serialInputBuffer[serialInputCount],HEX);
      serialInputCount++;
    }
  }

  //The minimum length received for a valid data packet will be 6 bytes
  //Lets start checking the data as soon as we received 6 bytes
  if (serialInputCount >= 6){   

    // move the first 4 bytes to the serial input Header
    memcpy(&serialInputHeader,&serialInputBuffer[0],4);
    if ((serialInputHeader.Length + 5) <= serialInputCount){
      // We received all the data required for this packet
      // calculate the CRC of the received packet
      calc_crc = createCRC(&serialInputBuffer[0], (serialInputHeader.Length + 3));

      // get the received CRC
      memcpy(&recv_crc,&serialInputBuffer[serialInputHeader.Length + 3],2);
      Serial.println(calc_crc); 

      // compare the received and calculated CRC values
      if (calc_crc == recv_crc){
        // all checks out and we can use the data
        if (report == 1){
          Serial.print("Data packet received with ID: ");
          Serial.println(serialInputHeader.ID,DEC);
        }
        // lets assume for now there is no other data in the input buffer
        // and then clear the counter
        serialInputCount = 0;

        //we have now valid data to process
        switch (serialInputHeader.ID){
          case 0:// Product Name (16 bytes string)
              //move the data from the buffer to the variable
              memcpy(&HardwareModel[0],&serialInputBuffer[4],16);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Hardware Model: ");
                Serial.write(HardwareModel,16);
                Serial.println(" ");
              }
              new_data = 1;
            break;
          case 1:// Hardware Version (4 bytes)
              //move the data from the buffer to the variable
              memcpy(&HardwareVersion,&serialInputBuffer[4],4);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Hardware Version: ");
                Serial.print(HardwareVersion,DEC);
                Serial.println(" ");
              }
              new_data = 1;
            break;
          case 2: // Frimware Version (4 bytes)
              //move the data from the buffer to the variable
              FirmwareVersionPatch = serialInputBuffer[4];
              FirmwareVersionMinor = serialInputBuffer[5];
              FirmwareVersionMajor = serialInputBuffer[6];
              FirmwareVersionReserved = serialInputBuffer[7];
              // lets print the received data if required
              if (report == 1){
                Serial.print("Firmware Version: ");
                Serial.print(FirmwareVersionMajor,DEC);
                Serial.print(".");
                Serial.print(FirmwareVersionMinor,DEC);
                Serial.print(".");
                Serial.println(FirmwareVersionPatch,DEC);
              }
              new_data = 1;
            break;
          case 3: // Serial Number (16 bytes string)
              //move the data from the buffer to the variable
              memcpy(&SerialNumber[0],&serialInputBuffer[4],16);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Serial Number: ");
                Serial.write(SerialNumber,16);
                Serial.println(" ");
              }
              new_data = 1;
            break;
          case 10: // Token (2 bytes)
              //move the data from the buffer to the variable
              memcpy(&Token,&serialInputBuffer[4],2);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Token: ");
                Serial.println(Token,DEC);
              }
              new_data = 1;
            break;
          case 12: // Save Parameters (none)
              // lets print the received data if required
              if (report == 1){
                Serial.println("Save Parameter Return Recieved");
              }
            break;
          case 14: // Reset (none)
              // lets print the received data if required
              if (report == 1){
                Serial.println("Save Parameter Return Recieved");
              }
            break;
          case 27: // Distance output configuration (4 bytes)
              //move the data from the buffer to the variable
              memcpy(&DistanceOutConfig,&serialInputBuffer[4],4);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Distance output Config: ");
                Serial.println(DistanceOutConfig,DEC);
              }
              new_data = 1;
            break;
          case 30: // Current Data Stream Type (4 bytes)
              //move the data from the buffer to the variable
              memcpy(&DataStreamType,&serialInputBuffer[4],4);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Data Stream Type: ");
                Serial.println(DataStreamType,DEC);
              }
              new_data = 1;
            break;
          // case 40: // Raw Streaming Data (varies number of bytes)
          //     data_cnt = 4;
          //     //move the data from the buffer to the variable
          //     raw_stream_cnt = serialInputBuffer[data_cnt++];
          //     memcpy(&RawDataStream[0],&serialInputBuffer[data_cnt],raw_stream_cnt*2);
          //     // lets print the received data if required
          //     if (report == 1){
          //       for (i = 0; i < raw_stream_cnt; i++){
          //         Serial.println(RawDataStream[i],DEC);
          //       }
          //     }
          //     new_data = 1;
          //   break;
          case 44: // Distance Data (varies number of bytes)
              //we need to run through the Distance Output Configuration bits to know what
              //data is being sent to us
              data_cnt = 4;
              for (i = 0; i < 16; i++){
                //Serial.print(i);
                //Serial.println(" ");
                if ((tempSel & 1) == 1){
                  switch (i){
                    case 0:
                      firstRaw_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(firstRaw_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 1:
                      firstFiltered_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(firstFiltered_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 2:
                      firstStrength_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(firstStrength_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 3:
                      lastRaw_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(lastRaw_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 4:
                      lastFiltered_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(lastFiltered_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 5:
                      lastStrength_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(lastStrength_cm);
                        Serial.print(" ");
                      }
                      break;
                    case 6:
                      backgroundNoise = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(backgroundNoise);
                        Serial.print(" ");
                      }
                      break;
                    case 7:
                      APDTemperature = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8);
                      if (report == 1){
                        Serial.print(APDTemperature);
                        Serial.print(" ");
                      }
                      break;
                    default:
                      break;
                  }
                }
                tempSel >>= 1;
              }
              if (report == 1){
                Serial.println(" ");
              }
              new_data = 1;
            break;
          case 57: // APD Temperature (4 byte)
              //move the data from the buffer to the variable
              memcpy(&APDTemperature,&serialInputBuffer[4],4);
              // lets print the received data if required
              if (report == 1){
                Serial.print("APD Temperature: ");
                Serial.println(APDTemperature,DEC);
              }
              new_data = 1;
            break;  
          case 79: // Serial BaudRate (1 byte)
              //move the data from the buffer to the variable
              serialBaudrate = serialInputBuffer[4];
              // lets print the received data if required
              if (report == 1){
                Serial.print("Serial Baudrate: ");
                Serial.println(serialBaudTable[serialBaudrate],DEC);
              }
              new_data = 1;
            break;
          case 80: // I2C address (1 byte)
              //move the data from the buffer to the variable
              I2CAddress = serialInputBuffer[4];
              // lets print the received data if required
              if (report == 1){
                Serial.print("I2C Address: ");
                Serial.println(I2CAddress,HEX);
              }
              new_data = 1;
            break;
          case 66: // Sampling Rate (1 byte)
              //move the data from the buffer to the variable
              memcpy(&SamplingRate,&serialInputBuffer[4],1);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Sampling Rate: ");
                Serial.println(SamplingRate,DEC);
              }
              new_data = 1;
            break; 
          case 200: // Signal probability Data (?????)(200 and 201)
              data_cnt = 4;
              //move the data from the buffer to the variable
              StatBucketCount = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
                                 | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
              memcpy(&StatBucketsFront[0],&serialInputBuffer[data_cnt],StatBucketCount*2);
              data_cnt += StatBucketCount*2;
              memcpy(&StatBucketsRear[0],&serialInputBuffer[data_cnt],StatBucketCount*2);
              data_cnt += StatBucketCount*2;
              StatShotCount = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
                               | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
              // lets print the received data if required
              if (report == 1){
                Serial.print("Statistic Bucket Count: ");
                Serial.println(StatBucketCount,DEC);
                Serial.print("Statistic Shot Count: ");
                Serial.println(StatShotCount,DEC);
                for (i = 0; i < StatBucketCount; i++){
                  Serial.print(i,DEC);
                  Serial.print(" ");
                  Serial.print(StatBucketsFront[i],DEC);
                  Serial.print(" ");
                  Serial.println(StatBucketsRear[i],DEC);
                }
              }
              new_data = 1;
            break;
          // case 202: // Distance Data in mm(varies number of bytes)
          //     //we need to run through the Distance Output Configuration bits to know what
          //     //data is being sent to us
          //     data_cnt = 4;
          //     for (i = 0; i < 16; i++){
          //       //Serial.print(i);
          //       //Serial.println(" ");
          //       if ((tempSel & 1) == 1){
          //         switch (i){
          //           case 0:
          //             firstRaw_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                             | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(firstRaw_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 1:
          //             firstFiltered_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                                 | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(firstFiltered_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 2:
          //             firstStrength_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                                 | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(firstStrength_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 3:
          //             lastRaw_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                           | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(lastRaw_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 4:
          //             lastFiltered_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                               | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(lastFiltered_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 5:
          //             lastStrength_mm = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                                 | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(lastStrength_mm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 6:
          //             backgroundNoise = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                                   | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(backgroundNoise);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 7:
          //             APDTemperature = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8)
          //                                   | ((uint32_t)serialInputBuffer[data_cnt++] << 16) | ((uint32_t)serialInputBuffer[data_cnt++] << 24);
          //             if (report == 1){
          //               Serial.print(APDTemperature);
          //               Serial.print(" ");
          //             }
          //             break;
          //           default:
          //             break;
          //         }
          //       }
          //       tempSel >>= 1;
          //     }
          //     if (report == 1){
          //       Serial.println(" ");
          //     }
          //     new_data = 1;
          //   break;
          // case 204: // Distance Data in cm (varies number of bytes)
          //     //we need to run through the Distance Output Configuration bits to know what
          //     //data is being sent to us
          //     data_cnt = 4;
          //     for (i = 0; i < 16; i++){
          //       //Serial.print(i);
          //       //Serial.println(" ");
          //       if ((tempSel & 1) == 1){
          //         switch (i){
          //           case 0:
          //             firstRaw_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(firstRaw_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 1:
          //             firstFiltered_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(firstFiltered_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 2:
          //             firstStrength_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(firstStrength_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 3:
          //             lastRaw_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(lastRaw_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 4:
          //             lastFiltered_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(lastFiltered_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 5:
          //             lastStrength_cm = ((uint16_t)serialInputBuffer[data_cnt++] << 0) | ((uint16_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(lastStrength_cm);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 6:
          //             backgroundNoise = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(backgroundNoise);
          //               Serial.print(" ");
          //             }
          //             break;
          //           case 7:
          //             APDTemperature = ((uint32_t)serialInputBuffer[data_cnt++] << 0) | ((uint32_t)serialInputBuffer[data_cnt++] << 8);
          //             if (report == 1){
          //               Serial.print(APDTemperature);
          //               Serial.print(" ");
          //             }
          //             break;
          //           default:
          //             break;
          //         }
          //       }
          //       tempSel >>= 1;
          //     }
          //     if (report == 1){
          //       Serial.println(" ");
          //     }
          //     new_data = 1;
          //   break;
          default:
            Serial.println("Unknown packet ID received");
            break;
        }
      }
      else {
        //we failed the crc so the data is corrupted. Lets clear the buffer.
        serialInputCount = 0;
      }
    }
  }

  return new_data;
}


//----------------------------------------
//------- I2C Interface ------------------
//----------------------------------------

uint32_t LW_SF000::i2cReadSelectDistanceOutputValues(int16_t report) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(27);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)4);
  uint32_t distanceOutput = ((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8) | ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 24);

  if (report == 1){
    Serial.print("Distance output: ");
    Serial.println(distanceOutput);
  }
  Wire.endTransmission();
  return distanceOutput;
  DistanceOutConfig = distanceOutput;
}

void LW_SF000::i2cWriteSelectDistanceOutputValues(uint32_t Value) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(27);    // ID

  //When Value is set to 0, the selected display outputs is set below,
  //otherwise when Value is not 0, it will dictate which outputs.
  //The following bits must be set to output value:
  //bit0 - First Return Raw
  //bit1 - First Return Filtered
  //bit2 - First Return Strength
  //bit3 - Last Return Raw
  //bit4 - Last Return Filtered
  //bit5 - Last Return Strength
  //bit6 - Background Noise
  //bit7 - Temperature
  //bit8 - Yaw Angle

  // if no setting was passed to the function, we use the following:
  if (Value == 0){
    // comment ones not the be displayed
    Value |= 0x01;  //bit0 - First Return Raw
    //Value |= 0x02;  //bit1 - First Return Filtered
    Value |= 0x04;  //bit2 - First Return Strength
    Value |= 0x08;  //bit3 - Last Return Raw
    //Value |= 0x10;  //bit4 - Last Return Filtered
    Value |= 0x20;  //bit5 - Last Return Strength
    Value |= 0x40;  //bit6 - Background Noise
    Value |= 0x80;  //bit7 - Temperature
    Value |= 0x100; //bit8 - Yaw Angle
  }

  Serial.println((Value >> 0) & 0xFF);
  Serial.println((Value >> 8) & 0xFF);
  Serial.println((Value >> 16) & 0xFF);
  Serial.println((Value >> 24) & 0xFF);

  Wire.write((Value >> 0) & 0xFF);
  Wire.write((Value >> 8) & 0xFF);
  Wire.write((Value >> 16) & 0xFF);
  Wire.write((Value >> 24) & 0xFF);
  Wire.endTransmission();

  DistanceOutConfig = Value;
}

// This function reads which distance outputs are selected and then read
// them from the SF000/D.
// They are then printed in sequence on the serial port.
void LW_SF000::i2cReadDistanceOutputValues(int16_t report) {
  int16_t i = 0;
  uint16_t temp = 0;

  //Read from the SF000/D which outputs are selected for distance
  uint32_t selectedDistanceOutput = i2cReadSelectDistanceOutputValues(0);

  //Count the number of selected outputs
  int16_t selectedCount = 0;
  uint32_t tempSel = selectedDistanceOutput;
  for (i = 0; i < 16; i++){
      selectedCount += tempSel & 1;
      tempSel >>= 1;
  }

  if (selectedCount == 0){
    Serial.println("No distance outputs selected");
  }
  else {
    tempSel = selectedDistanceOutput;
    Wire.beginTransmission(I2CAddress);
    Wire.write(44);    // ID
    Wire.endTransmission(false);
    Wire.requestFrom(I2CAddress, (uint8_t)(selectedCount*2));
    for (i = 0; i < 16; i++){
      //Serial.print(i);
      //Serial.println(" ");
      if ((tempSel & 1) == 1){
        switch (i){
          case 0:
            firstRaw_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
			        Serial.print(firstRaw_cm);
			        Serial.print(" ");
			      }
            break;
          case 1:
            firstFiltered_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
              Serial.print(firstFiltered_cm);
              Serial.print(" ");
		  	    }
            break;
          case 2:
            firstStrength_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
			        Serial.print(firstStrength_cm);
			        Serial.print(" ");
			      }
            break;
          case 3:
            lastRaw_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
			        Serial.print(lastRaw_cm);
			        Serial.print(" ");
			      }
            break;
          case 4:
            lastFiltered_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
              Serial.print(lastFiltered_cm);
              Serial.print(" ");
            }
            break;
          case 5:
            lastStrength_cm = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);
            if (report == 1){
              Serial.print(lastStrength_cm);
              Serial.print(" ");
            }
            break;
          case 6:
            backgroundNoise = ((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8);
            if (report == 1){
              Serial.print(backgroundNoise);
              Serial.print(" ");
            }
            break;
          case 7:
            APDTemperature = ((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8);
            if (report == 1){
              Serial.print(APDTemperature);
              Serial.print(" ");
            }
            break;
          default:
            break;
        }
      }
      tempSel >>= 1;
    }
    Wire.endTransmission();
    if (report == 1){
      Serial.println("");
    }
  }
}

// This function reads the Hardware model then save it in the global variable
void LW_SF000::i2cReadHardwareModel(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(0);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)16);
  if (report == 1){
    Serial.print("HardwareModel: ");
  }
  for (i = 0; i < 16; i++){
    HardwareModel[i] = (char)Wire.read(); //
    // print the values if report = 1
    // if (report == 1){
    //   Serial.print(HardwareModel[i]);
    // }
  }
  if (report == 1){
    Serial.write(HardwareModel,16);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the Hardware Version then save it in the global variable
void LW_SF000::i2cReadHardwareVersion(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(1);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)4);

  HardwareVersion = ((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8) | ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 24);

  if (report == 1){
    Serial.print("HardwareVersion: ");
    Serial.print(HardwareVersion);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the Firmware Version then save it in the global variable
void LW_SF000::i2cReadFirmwareVersion(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(2);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)4);

  FirmwareVersionPatch = Wire.read();
  FirmwareVersionMinor = Wire.read();
  FirmwareVersionMajor = Wire.read();
  FirmwareVersionReserved = Wire.read();

  if (report == 1){
    Serial.print("FirmwareVersion: ");
    Serial.print(FirmwareVersionMajor);
    Serial.print(".");
    Serial.print(FirmwareVersionMinor);
    Serial.print(".");
    Serial.print(FirmwareVersionPatch);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the Serial Number then save it in the global variable
void LW_SF000::i2cReadSerialNumber(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(3);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)16);
  if (report == 1){
    Serial.print("Serial Number: ");
  }
  for (i = 0; i < 16; i++){
    SerialNumber[i] = (char)Wire.read();
    // print the values if report = 1
    if (report == 1){
      Serial.print(SerialNumber[i]);
    }
  }
  if (report == 1){
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the Token and saves it to the global variables
void LW_SF000::i2cReadToken(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(10);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)2);

  Token = ((uint16_t)Wire.read() << 0) | ((uint16_t)Wire.read() << 8);

  if (report == 1){
    Serial.print("Token: ");
    Serial.print(Token);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the APD TEmperature and saves it to the global variables
void LW_SF000::i2cReadAPDTemp(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(57);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)4);

  APDTemperature = (int32_t)((uint32_t)Wire.read() << 0) | ((uint32_t)Wire.read() << 8) | ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 24);

  if (report == 1){
    Serial.print("APD Temperature: ");
    Serial.print(APDTemperature);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This function reads the Token and saves it to the global variables
void LW_SF000::i2cReadSamplingRate(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(66);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)1);

  SamplingRate = (uint8_t)Wire.read();

  if (report == 1){
    Serial.print("Sampling Rate: ");
    Serial.print(SamplingRate);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This will send a save command.
// A new token must be requested and recieved and then passed to this
// function to send with the command
void LW_SF000::i2cWriteSaveParameters(uint16_t Token) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(12);    // ID

  Wire.write((Token >> 0) & 0xFF);
  Wire.write((Token >> 8) & 0xFF);
  Wire.endTransmission();
}

// This will send a Reset command.
// A new token must be requested and recieved and then passed to this
// function to send with the command
void LW_SF000::i2cWriteResetCommand(uint16_t Token) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(14);    // ID

  Wire.write((Token >> 0) & 0xFF);
  Wire.write((Token >> 8) & 0xFF);
  Wire.endTransmission();
}

// This will change the Serial Baudrate
void LW_SF000::i2cWriteSerialBaudrate(uint8_t Rate) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(79);    // ID

  Wire.write(Rate);
  Wire.endTransmission();

  serialBaudrate = Rate;
}

// This will request the current I2C Address
void LW_SF000::i2cReadI2CAddress(int16_t report) {
  int16_t i = 0;
  Wire.beginTransmission(I2CAddress);
  Wire.write(80);    // ID
  Wire.endTransmission(false);
  Wire.requestFrom(I2CAddress, (uint8_t)1);

  I2CAddress = (uint8_t)Wire.read();

  if (report == 1){
    Serial.print("I2C Address: ");
    Serial.print(I2CAddress,HEX);
    Serial.println(" ");
  }
  Wire.endTransmission();
}

// This will change the I2C interface Address
void LW_SF000::i2cWriteI2CAddress(uint8_t Address) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(80);    // ID

  Wire.write(Address);
  Wire.endTransmission();

  I2CAddress = Address;
}

// This will change the Sampling Rate
void LW_SF000::i2cWriteSamplingRate(uint8_t Rate) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(66);    // ID

  Wire.write(Rate);
  Wire.endTransmission();

  SamplingRate = (uint8_t)Rate;
}

// This enable/disable Median Filter
void LW_SF000::i2cWriteMedianEnable(uint8_t En) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(82);    // ID

  Wire.write(En);
  Wire.endTransmission();
}

// This enable/disable Smoothing Filter
void LW_SF000::i2cWriteSmoothingEnable(uint8_t En) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(84);    // ID

  Wire.write(En);
  Wire.endTransmission();
}

// This will change the Median Filter
void LW_SF000::i2cWriteMedianFilter(uint32_t Filter) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(83);    // ID

  Wire.write((Filter >> 0) & 0xFF);
  Wire.write((Filter >> 8) & 0xFF);
  Wire.write((Filter >> 16) & 0xFF);
  Wire.write((Filter >> 24) & 0xFF);
  Wire.endTransmission();
}

// This will change the Median Filter
void LW_SF000::i2cWriteSmoothingFilter(uint32_t Filter) {
  Wire.beginTransmission(I2CAddress);
  Wire.write(85);    // ID

  Wire.write((Filter >> 0) & 0xFF);
  Wire.write((Filter >> 8) & 0xFF);
  Wire.write((Filter >> 16) & 0xFF);
  Wire.write((Filter >> 24) & 0xFF);
  Wire.endTransmission();
}