#include "lwnx.h"
#include <string.h>

uint16_t lwnxCreateCrc(uint8_t* Data, uint16_t Size) {
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

void lwnxInitResponsePacket(lwResponsePacket* Response) {
	Response->size = 0;
	Response->payloadSize = 0;
	Response->parseState = 0;
}

uint8_t lwnxParseData(lwResponsePacket* Response, uint8_t Data) {
	if (Response->parseState == 0) {
		if (Data == PACKET_START_BYTE) {
			Response->parseState = 1;
			Response->data[0] = PACKET_START_BYTE;
		}
	} else if (Response->parseState == 1) {
		Response->parseState = 2;
		Response->data[1] = Data;
	} else if (Response->parseState == 2) {
		Response->parseState = 3;
		Response->data[2] = Data;
		Response->payloadSize = (Response->data[1] | (Response->data[2] << 8)) >> 6;
		Response->payloadSize += 2;
		Response->size = 3;

		if (Response->payloadSize > 1019) {
			Response->parseState = 0;
			// NOTE: Packet too long.
		}
	} else if (Response->parseState == 3) {
		Response->data[Response->size++] = Data;

		if (--Response->payloadSize == 0) {
			Response->parseState = 0;
			uint16_t crc = Response->data[Response->size - 2] | (Response->data[Response->size - 1] << 8);
			uint16_t verifyCrc = lwnxCreateCrc(Response->data, Response->size - 2);

			if (crc == verifyCrc) {
				Response->parseState = 0;
				return 1;
			} else {
				Response->parseState = 0;
				// NOTE: Packet has invalid CRC.
			}
		}
	}

	return 0;
}

uint8_t lwnxRecvPacketNoBlock(lwEndpoint* Endpoint, uint8_t CommandId, lwResponsePacket* Response) {
	uint8_t byte = 0;
	int32_t bytesRead = Endpoint->readCallback(&byte, 1);

	if (bytesRead != 0) {
		if (lwnxParseData(Response, byte)) {
			int8_t cmdId = Response->data[3];
			
			if (cmdId == CommandId) {
				return 1;
			}
		}
	}

	return 0;
}

uint8_t lwnxRecvPacket(lwEndpoint* Endpoint, uint8_t CommandId, lwResponsePacket* Response, uint32_t TimeoutMs) {
	lwnxInitResponsePacket(Response);

	uint32_t timeoutTime = Endpoint->timeCallback() + TimeoutMs;

	while (Endpoint->timeCallback() < timeoutTime) {
		uint8_t byte = 0;
		int32_t bytesRead = 0;
		
		while ((bytesRead = Endpoint->readCallback(&byte, 1)) > 0) {
			if (lwnxParseData(Response, byte)) {
				int8_t cmdId = Response->data[3];
				
				if (cmdId == CommandId) {
					return 1;
				}
			}
		}
	}

	return 0;
}

uint8_t lwnxRecvPacketAny(lwEndpoint* Endpoint, lwResponsePacket* Response, uint32_t TimeoutMs) {
	lwnxInitResponsePacket(Response);

	uint32_t timeoutTime = Endpoint->timeCallback() + TimeoutMs;

	while (Endpoint->timeCallback() < timeoutTime) {
		uint8_t byte = 0;
		int32_t bytesRead = 0;
		
		while ((bytesRead = Endpoint->readCallback(&byte, 1)) > 0) {
			if (lwnxParseData(Response, byte)) {
				int8_t cmdId = Response->data[3];
				Response->cmdId = cmdId;
				return 1;
			}
		}
	}

	return 0;
}

void lwnxSendPacketBytes(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t Write, uint8_t* Data, uint32_t DataSize) {
	uint8_t buffer[1024];
	uint32_t payloadLength = 1 + DataSize;
	uint16_t flags = (payloadLength << 6) | (Write & 0x1);
	
	buffer[0] = PACKET_START_BYTE;					// Start byte.
	buffer[1] = ((uint8_t*)&flags)[0];				// Flags low.
	buffer[2] = ((uint8_t*)&flags)[1];				// Flags high.
	buffer[3] = CommandId;							// Payload: Command ID.
	memcpy(buffer + 4, Data, DataSize);				// Payload: Data.
	uint16_t crc = lwnxCreateCrc(buffer, 4 + DataSize);
	buffer[4 + DataSize] = ((uint8_t*)&crc)[0];		// Checksum low.
	buffer[5 + DataSize] = ((uint8_t*)&crc)[1];		// Checksum high.

	Endpoint->writeCallback(buffer, 6 + DataSize);
}

uint8_t lwnxHandleManagedCmd(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response, uint32_t ResponseSize, uint8_t Write, uint8_t* RequestData, uint32_t RequestSize) {
	int32_t attempts = PACKET_RETRIES;

	while (attempts--) {
		lwnxSendPacketBytes(Endpoint, CommandId, Write, RequestData, RequestSize);

		lwResponsePacket response = {};
		
		if (lwnxRecvPacket(Endpoint, CommandId, &response, PACKET_TIMEOUT)) {
			uint32_t copySize = ResponseSize;

			if (response.size - 6 < ResponseSize) {
				copySize = response.size - 6;
			}
			
			memcpy(Response, response.data + 4, copySize);
			
			return 1;
		}
	}

	return 0;
}

uint8_t lwnxCmdReadInt8(lwEndpoint* Endpoint, uint8_t CommandId, int8_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 1, 0, 0, 0);
}

uint8_t lwnxCmdReadInt16(lwEndpoint* Endpoint, uint8_t CommandId, int16_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 2, 0, 0, 0);
}

uint8_t lwnxCmdReadInt32(lwEndpoint* Endpoint, uint8_t CommandId, int32_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 4, 0, 0, 0);
}

uint8_t lwnxCmdReadUInt8(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 1, 0, 0, 0);
}

uint8_t lwnxCmdReadUInt16(lwEndpoint* Endpoint, uint8_t CommandId, uint16_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 2, 0, 0, 0);
}

uint8_t lwnxCmdReadUInt32(lwEndpoint* Endpoint, uint8_t CommandId, uint32_t* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 4, 0, 0, 0);
}

uint8_t lwnxCmdReadString(lwEndpoint* Endpoint, uint8_t CommandId, char* Response) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, (uint8_t*)Response, 16, 0, 0, 0);
}

uint8_t lwnxCmdReadData(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Response, uint32_t ResponseSize) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, Response, ResponseSize, 0, 0, 0);
}

uint8_t lwnxCmdWriteInt8(lwEndpoint* Endpoint, uint8_t CommandId, int8_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 1);
}

uint8_t lwnxCmdWriteInt16(lwEndpoint* Endpoint, uint8_t CommandId, int16_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 2);
}

uint8_t lwnxCmdWriteInt32(lwEndpoint* Endpoint, uint8_t CommandId, int32_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 4);
}

uint8_t lwnxCmdWriteUInt8(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 1);
}

uint8_t lwnxCmdWriteUInt16(lwEndpoint* Endpoint, uint8_t CommandId, uint16_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 2);
}

uint8_t lwnxCmdWriteUInt32(lwEndpoint* Endpoint, uint8_t CommandId, uint32_t Value) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)&Value, 4);
}

uint8_t lwnxCmdWriteString(lwEndpoint* Endpoint, uint8_t CommandId, char* String) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, (uint8_t*)String, 16);
}

uint8_t lwnxCmdWriteData(lwEndpoint* Endpoint, uint8_t CommandId, uint8_t* Data, uint32_t DataSize) {
	return lwnxHandleManagedCmd(Endpoint, CommandId, 0, 0, 1, Data, DataSize);
}