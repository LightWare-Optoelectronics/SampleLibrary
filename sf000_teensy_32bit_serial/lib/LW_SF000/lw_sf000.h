//----------------------------------------
// www.Lightware.co.za          MArch 2021
// Serial and I2C interface for the SF000
// Modified by Andrw Yang (@druyang)
//----------------------------------------

// TO DO
// Must still add the Read Sampling Rate function

#ifndef LW_SF000_H
#define LW_SF000_H

#include <inttypes.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>

class LW_SF000{
	public:
		LW_SF000(HardwareSerial &serial1);
		
		void test(void);
		//LW_SF000();
		// This will request the product Hardware Name
		void readRequestHardwareName(void);

		// This will request the product Hardware Version Number
		void readRequestHardwareVersion(void);

		// This will request the product Firmware Version number
		void readRequestFirmwareVersion(void);

		// This will request the product Serial Number
		void readRequestSerialNumber(void);

		// This will request the next usable Token
		void readRequestToken(void);

		// This will request the current Distance Output Configuration settings
		void readRequestDistOutConfig(void);

		// This will request the current Data Stream Type setting
		void readRequestDataStreamType(void);

		// This will request the current Serial interface Baudrate
		void readRequestSerialBaudrate(void);

		// This will request the current Distance Output as per the Distance output Configuration settings
		void readRequestDistance(void);

		// This will request the current I2C Address
		void readRequestI2CAddress(void);

		// This will request the APD Temperature
		void readRequestAPDTemp(void);

		// This will request the Sampling Rate
		void readRequestSamplingRate(void);

		// This will send a save command.
		// A new token must be requested and recieved and then passed to this
		// function to send with the command
		void writeSaveParameters(uint16_t Token);

		// This will send a Reset command.
		// A new token must be requested and recieved and then passed to this
		// function to send with the command
		void writeResetCommand(uint16_t Token);

		// This will change the Distance Output configuration
		void writeDistOutConfig(uint32_t Value);

		// This will change the Data Stream output Type
		void writeDataStreamType(uint32_t Type);

		// This will change the Serial Baudrate
		void writeSerialBaudrate(uint8_t Rate);

		// This will change the I2C interface Address
		void writeI2CAddress(uint8_t Address);

		// This will change the Sampling Rate
		void writeSamplingRate(uint8_t Rate);

		// Read data sent back from the SF000/D
		uint8_t ProcessSerialInput(int16_t report);

		//----------------------------------------
		//------- I2C Interface ------------------
		//----------------------------------------

		uint32_t i2cReadSelectDistanceOutputValues(int16_t report);

		void i2cWriteSelectDistanceOutputValues(uint32_t Value);

		// This function reads which distance outputs are selected and then read
		// them from the SF000/D.
		// They are then printed in sequence on the serial port.
		void i2cReadDistanceOutputValues(int16_t report);

		// This function reads the Hardware model then save it in the global variable
		void i2cReadHardwareModel(int16_t report);

		// This function reads the Hardware Version then save it in the global variable
		void i2cReadHardwareVersion(int16_t report);

		// This function reads the Firmware Version then save it in the global variable
		void i2cReadFirmwareVersion(int16_t report);

		// This function reads the Serial Number then save it in the global variable
		void i2cReadSerialNumber(int16_t report);



		// This will request the next usable Token
		void i2cReadToken(int16_t report);

		// This will request the APD Temperature
		void i2cReadAPDTemp(int16_t report);

		// This will request the Sampling Rate
		void i2cReadSamplingRate(int16_t report);

		// This will send a save command.
		// A new token must be requested and recieved and then passed to this
		// function to send with the command
		void i2cWriteSaveParameters(uint16_t Token);

		// This will send a Reset command.
		// A new token must be requested and recieved and then passed to this
		// function to send with the command
		void i2cWriteResetCommand(uint16_t Token);

		// This will change the Serial Baudrate
		void i2cWriteSerialBaudrate(uint8_t Rate);

		// This will request the current I2C Address
		void i2cReadI2CAddress(int16_t report);

		// This will change the I2C interface Address
		void i2cWriteI2CAddress(uint8_t Address);

		// This will change the Sampling Rate
		void i2cWriteSamplingRate(uint8_t Rate);

		// This enable/disable Median Filter
		void LW_SF000::i2cWriteMedianEnable(uint8_t En); 

		// This enable/disable Smoothing Filter
		void LW_SF000::i2cWriteSmoothingEnable(uint8_t En);

		// This will change the Median Filter
		void LW_SF000::i2cWriteMedianFilter(uint32_t Filter);

		// This will change the Median Filter
		void LW_SF000::i2cWriteSmoothingFilter(uint32_t Filter);		

		//----------------------------------------
		//----------- Global variables -----------
		//----------------------------------------
		int16_t firstRaw_cm = 0;
		int16_t firstFiltered_cm = 0;
		int16_t firstStrength_cm = 0;
		int16_t lastRaw_cm = 0;
		int16_t lastFiltered_cm = 0;
		int16_t lastStrength_cm = 0;
		int32_t firstRaw_mm = 0;
		int32_t firstFiltered_mm = 0;
		int32_t firstStrength_mm = 0;
		int32_t lastRaw_mm = 0;
		int32_t lastFiltered_mm = 0;
		int32_t lastStrength_mm = 0;
		int32_t backgroundNoise = 0;
		char HardwareModel[16] = {};
		uint32_t HardwareVersion = 0;
		uint8_t FirmwareVersionReserved = 0;
		uint8_t FirmwareVersionMajor = 0;
		uint8_t FirmwareVersionMinor = 0;
		uint8_t FirmwareVersionPatch = 0;
		char SerialNumber[16] = {};
		uint16_t Token = 0;
		uint32_t DistanceOutConfig = 1;
		uint32_t DataStreamType = 0;
		uint8_t serialBaudrate = 4;
		uint8_t I2CAddress = 0x66;
		int16_t RawDataStream[210] = {};
		uint32_t StatBucketCount = 0;
		uint32_t StatShotCount = 0;
		uint16_t StatBucketsFront[130] = {};
		uint16_t StatBucketsRear[130] = {};
		int32_t APDTemperature = 0;
		int8_t SamplingRate = 0;

	private:
		HardwareSerial* Serial1;
		//----------------------------------------
		//------- Serial Interface ---------------
		//----------------------------------------
		// Default Baudrate:
		// 115200
		// Other Available Baudrates:
		// 9600, 19200, 38400, 57600, 230400, 460800, 921600
		const int32_t serialBaudTable[9] = { 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1440000 };

		// This function creates the CRC for the Serial data
		uint16_t createCRC(uint8_t* Data, uint16_t Size);

		typedef struct __attribute__((packed)){
		  uint8_t Start         : 8;
		  uint16_t Write_Read   : 1;
		  uint16_t Reserved     : 5;
		  uint16_t Length       : 10;
		  uint8_t ID            : 8;
		} Header_struct;

		Header_struct serialOutputHeader = {};
		Header_struct serialInputHeader = {};

		// serial write communcation buffer with max length 1030
		uint8_t serialOutputBuffer[1030] = {};
		// number of bytes used in buffer to write
		uint16_t serialOutputCount = 0;

		// serial read communcation buffer
		uint8_t serialInputBuffer[2049] = {};
		// number of bytes read
		uint16_t serialInputCount = 0;
};

extern TwoWire Wire;

#endif