#pragma once

#include "common.h"

class lwSerialPort {
	private:
		Stream* _port;

	public:
		bool connect(Stream* Port, int BitRate);
		bool disconnect();
		int writeData(uint8_t *Buffer, int32_t BufferSize);
		int32_t readData(uint8_t *Buffer, int32_t BufferSize);
};