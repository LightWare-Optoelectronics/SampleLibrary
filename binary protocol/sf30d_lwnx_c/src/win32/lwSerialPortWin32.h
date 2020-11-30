#pragma once

#include "platformWin32.h"

class lwSerialPortWin32 : public lwSerialPort {
	private:
		HANDLE _descriptor;

	public:
		bool connect(const char* Name, int BitRate);
		bool disconnect();
		int writeData(uint8_t *Buffer, int32_t BufferSize);
		int32_t readData(uint8_t *Buffer, int32_t BufferSize);
};