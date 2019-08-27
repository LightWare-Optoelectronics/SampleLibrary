#ifndef __LWNX_H__
#define __LWNX_H__

#include <stdint.h>

#define PACKET_START_BYTE	0xAA
#define PACKET_TIMEOUT		200
#define PACKET_RETRIES		4

typedef int32_t (*writeCallbackFuncPtr)(uint8_t* Data, int32_t BufferSize);
typedef int32_t (*readCallbackFuncPtr)(uint8_t* Data, int32_t BufferSize);
typedef int32_t (*timeCallbackFuncPtr)();

typedef struct {
	writeCallbackFuncPtr writeCallback;
	readCallbackFuncPtr readCallback;
	timeCallbackFuncPtr timeCallback;

} lwEndpoint;

typedef struct {	
	uint8_t data[1024];
	int32_t size;
	int32_t payloadSize;
	uint8_t parseState;
	uint8_t cmdId;

} lwResponsePacket;

//----------------------------------------------------------------------------------------------------------------------------------
// Helper utilities.
//----------------------------------------------------------------------------------------------------------------------------------
// Create a CRC-16-CCITT 0x1021 hash of the specified data.
uint16_t lwnxCreateCrc(uint8_t* Data, uint16_t Size);

// Breaks an integer firmware version into Major, Minor, and Patch.
void lwnxConvertFirmwareVersionToStr(uint32_t Version, char* String);

//----------------------------------------------------------------------------------------------------------------------------------
// LWNX protocol implementation.
//----------------------------------------------------------------------------------------------------------------------------------
// Prepare a response packet for a new incoming response.
void lwnxInitResponsePacket(lwResponsePacket* Response);

// Waits to receive a packet of specific command id.
// Does not return until a response is received or a timeout occurs.
uint8_t lwnxRecvPacket(lwEndpoint* Endpoint, uint8_t CommandId, lwResponsePacket* Response, uint32_t TimeoutMs);

// Waits to receive any packet.
// Does not return until a response is received or a timeout occurs.
uint8_t lwnxRecvPacketAny(lwEndpoint* Endpoint, lwResponsePacket* Response, uint32_t TimeoutMs);

// Returns true if full packet was received, otherwise finishes immediately and returns false while waiting for more data.
uint8_t lwnxRecvPacketNoBlock(lwEndpoint* Endpoint, uint8_t CommandId, lwResponsePacket* Response);

// Composes and sends a packet.
void lwnxSendPacketBytes(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t Write, uint8_t* Data, uint32_t DataSize);

// Handle both the sending and receving of a command. 
// Does not return until a response is received or all retries have expired.
uint8_t lwnxHandleManagedCmd(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response, uint32_t ResponseSize, uint8_t Write, uint8_t* WriteData, uint32_t WriteSize);

//----------------------------------------------------------------------------------------------------------------------------------
// Command functions.
//----------------------------------------------------------------------------------------------------------------------------------
// Issue read commands.
uint8_t lwnxCmdReadInt8(lwEndpoint* Endpoint, uint8_t CommandId, int8_t* Response);
uint8_t lwnxCmdReadInt16(lwEndpoint* Endpoint, uint8_t CommandId, int16_t* Response);
uint8_t lwnxCmdReadInt32(lwEndpoint* Endpoint, uint8_t CommandId, int32_t* Response);

uint8_t lwnxCmdReadUInt8(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response);
uint8_t lwnxCmdReadUInt16(lwEndpoint* Endpoint, uint8_t CommandId, uint16_t* Response);
uint8_t lwnxCmdReadUInt32(lwEndpoint* Endpoint, uint8_t CommandId, uint32_t* Response);

uint8_t lwnxCmdReadString(lwEndpoint* Endpoint, uint8_t CommandId, char* Response);
uint8_t lwnxCmdReadData(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response, uint32_t ResponseSize);

// Issue write commands.
uint8_t lwnxCmdWriteInt8(lwEndpoint* Endpoint, uint8_t CommandId, int8_t Value);
uint8_t lwnxCmdWriteInt16(lwEndpoint* Endpoint, uint8_t CommandId, int16_t Value);
uint8_t lwnxCmdWriteInt32(lwEndpoint* Endpoint, uint8_t CommandId, int32_t Value);

uint8_t lwnxCmdWriteUInt8(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t Value);
uint8_t lwnxCmdWriteUInt16(lwEndpoint* Endpoint, uint8_t CommandId, uint16_t Value);
uint8_t lwnxCmdWriteUInt32(lwEndpoint* Endpoint, uint8_t CommandId, uint32_t Value);

uint8_t lwnxCmdWriteString(lwEndpoint* Endpoint, uint8_t CommandId, char* String);
uint8_t lwnxCmdWriteData(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Data, uint32_t DataSize);

#endif