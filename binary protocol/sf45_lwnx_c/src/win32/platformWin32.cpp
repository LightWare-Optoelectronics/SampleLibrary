#include "platformWin32.h"
#include "lwSerialPortWin32.h"

static int64_t timeFrequency;
static int64_t timeCounterStart;

void platformInit() {
	LARGE_INTEGER freq;
	LARGE_INTEGER counter;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&counter);
	timeCounterStart = counter.QuadPart;
	timeFrequency = freq.QuadPart;
}

double getTimeSeconds() {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	int64_t time = counter.QuadPart - timeCounterStart;
	double result = (double)time / ((double)timeFrequency);

	return result;
}

int64_t platformGetMicrosecond() {
	return (int64_t)(getTimeSeconds() * 1000000);
}

int32_t platformGetMillisecond() {
	return (int32_t)(getTimeSeconds() * 1000);
}

bool platformSleep(int32_t TimeMS) {
	Sleep(TimeMS);
	return true;
};

lwSerialPort* platformCreateSerialPort() {
	return new lwSerialPortWin32();
}
