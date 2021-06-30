#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "lwSerialPort.h"

#ifdef __linux__
	#include "./linux/platformLinux.h"
#else
	#include "./win32/platformWin32.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
void printHexDebug(uint8_t* Data, uint32_t Size);