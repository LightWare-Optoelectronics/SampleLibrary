#include "lwSerialPortWin32.h"

bool lwSerialPortWin32::connect(const char* Name, int BitRate) {
	_descriptor = INVALID_HANDLE_VALUE;
	printf("Attempt com connection: %s\n", Name);
			
	HANDLE handle = CreateFile(Name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("Serial Connect: Failed to open.\n");
		return false;
	}

	PurgeComm(handle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
		
	DCB comParams = {};
	comParams.DCBlength = sizeof(comParams);
	GetCommState(handle, &comParams);
	comParams.BaudRate = BitRate;
	comParams.ByteSize = 8;
	comParams.StopBits = ONESTOPBIT;
	comParams.Parity = NOPARITY;
	comParams.fDtrControl = DTR_CONTROL_ENABLE;
	comParams.fRtsControl = DTR_CONTROL_ENABLE;

	BOOL status = SetCommState(handle, &comParams);

	if (status == FALSE)
	{
		// NOTE: Some poorly written USB<->Serial drivers require the state to be set twice.
		status = SetCommState(handle, &comParams);

		if (status == FALSE)
		{
			disconnect();
			return false;
		}
	}

	COMMTIMEOUTS timeouts = {};
	GetCommTimeouts(handle, &timeouts);
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 10;
	//timeouts.WriteTotalTimeoutConstant = 0;
	//timeouts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(handle, &timeouts);

	_descriptor = handle;

	std::cout << "Serial Connect: Connected to " << Name << ".\n";

	return true;
}

bool lwSerialPortWin32::disconnect() {
	if (_descriptor != INVALID_HANDLE_VALUE)
		CloseHandle(_descriptor);

	_descriptor = INVALID_HANDLE_VALUE;
	
	return true;
}

int lwSerialPortWin32::writeData(uint8_t *Buffer, int32_t BufferSize) {
	if (_descriptor == INVALID_HANDLE_VALUE)
	{
		printf("Serial Write: Invalid Serial Port.\n");
		return -1;
	}

	OVERLAPPED overlapped = {};
	DWORD bytesWritten = 0;

	if (!WriteFile(_descriptor, Buffer, BufferSize, &bytesWritten, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			printf("Serial Write: Write Failed.\n");
			return -1;
		}
		else
		{
			if (!GetOverlappedResult(_descriptor, &overlapped, &bytesWritten, true))
			{
				printf("Serial Write: Waiting Error.\n");
				return -1;
			}
			else
			{
				if (bytesWritten != BufferSize)
					return -1;

				return bytesWritten;
			}
		}
	}
	else
	{
		if (bytesWritten != BufferSize)
			return -1;

		return bytesWritten;
	}

	return -1;
}

int32_t lwSerialPortWin32::readData(uint8_t *Buffer, int32_t BufferSize) {
	if (_descriptor == INVALID_HANDLE_VALUE)
	{
		printf("Serial Read: Invalid Serial Port.\n");
		return -1;
	}

	OVERLAPPED overlapped = {};
	DWORD bytesRead;

	if (!ReadFile(_descriptor, Buffer, BufferSize, &bytesRead, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			printf("Serial Read: IO Pending Error.\n");
			return -1;
		}
		else
		{
			if (!GetOverlappedResult(_descriptor, &overlapped, &bytesRead, true))
			{
				printf("Serial Read: Waiting Error.\n");
				return -1;
			}
			else if (bytesRead > 0)
			{
				return bytesRead;
			}
		}
	}
	else if (bytesRead > 0)
	{
		return bytesRead;
	}

	return 0;
}