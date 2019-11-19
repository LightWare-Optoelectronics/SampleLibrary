#--------------------------------------------------------------------------------------------------------------
# LightWare 2019
#--------------------------------------------------------------------------------------------------------------
# Description:
#   This samples communicates with LightWare devices that use the LWNX protocol.
#
# Notes:
# 	Requires the pySerial module.
#--------------------------------------------------------------------------------------------------------------

import time
import serial

#--------------------------------------------------------------------------------------------------------------
# LWNX library functions.
#--------------------------------------------------------------------------------------------------------------
packetParseState = 0
packetPayloadSize = 0
packetSize = 0
packetData = []

# Create a CRC-16-CCITT 0x1021 hash of the specified data.
def createCrc(data):
	crc = 0
	
	for i in data:
		code = crc >> 8
		code ^= int(i)
		code ^= code >> 4
		crc = crc << 8
		crc ^= code
		code = code << 5
		crc ^= code
		code = code << 7
		crc ^= code
		crc &= 0xFFFF

	return crc

# Create raw bytes for a packet.
def buildPacket(command, write, data=[]):
	payloadLength = 1 + len(data)
	flags = (payloadLength << 6) | (write & 0x1)
	packetBytes = [0xAA, flags & 0xFF, (flags >> 8) & 0xFF, command]
	packetBytes.extend(data)
	crc = createCrc(packetBytes)
	packetBytes.append(crc & 0xFF)
	packetBytes.append((crc >> 8) & 0xFF)

	return bytearray(packetBytes)

# Check for packet in byte stream.
def parsePacket(byte):
	global packetParseState
	global packetPayloadSize
	global packetSize
	global packetData

	if packetParseState == 0:
		if byte == 0xAA:
			packetParseState = 1
			packetData = [0xAA]

	elif packetParseState == 1:
		packetParseState = 2
		packetData.append(byte)

	elif packetParseState == 2:
		packetParseState = 3
		packetData.append(byte)
		packetPayloadSize = (packetData[1] | (packetData[2] << 8)) >> 6
		packetPayloadSize += 2
		packetSize = 3

		if packetPayloadSize > 1019:
			packetParseState = 0

	elif packetParseState == 3:
		packetData.append(byte)
		packetSize += 1
		packetPayloadSize -= 1

		if packetPayloadSize == 0:
			packetParseState = 0
			crc = packetData[packetSize - 2] | (packetData[packetSize - 1] << 8)
			verifyCrc = createCrc(packetData[0:-2])
			
			if crc == verifyCrc:
				return True

	return False

# Wait (up to timeout) for a packet to be received of the specified command.
def waitForPacket(port, command, timeout=1):
	global packetParseState
	global packetPayloadSize
	global packetSize
	global packetData

	packetParseState = 0
	packetData = []
	packetPayloadSize = 0
	packetSize = 0

	endTime = time.time() + timeout

	while True:
		if time.time() >= endTime:
			return None

		c = port.read(1)

		if len(c) != 0:
			b = ord(c)
			if parsePacket(b) == True:
				if packetData[3] == command:
					return packetData

# Extract a 16 byte string from a string packet.
def readStr16(packetData):
	str16 = ''
	for i in range(0, 16):
		if packetData[4 + i] == 0:
			break
		else:
			str16 += chr(packetData[4 + i])

	return str16

# Extract signal data from a signal data packet.
def readSignalData(packetData):
	signalFront = response[4 + 0] << 0
	signalFront |= response[4 + 1] << 8
	signalFront |= response[4 + 2] << 16
	signalFront |= response[4 + 3] << 24
	signalFront /= 1000.0

	signalRear = response[8 + 0] << 0
	signalRear |= response[8 + 1] << 8
	signalRear |= response[8 + 2] << 16
	signalRear |= response[8 + 3] << 24
	signalRear /= 1000.0

	currentDistance = response[12 + 0] << 0
	currentDistance |= response[12 + 1] << 8
	currentDistance |= response[12 + 2] << 16
	currentDistance |= response[12 + 3] << 24
	currentDistance /= 1000.0

	signalWidth = signalRear - signalFront

	return currentDistance, signalWidth

# Send a request packet and wait for response.
def executeCommand(port, command, write, data=[], timeout=1):
	packet = buildPacket(command, write, data)
	retries = 4

	while retries > 0:
		retries -= 1
		port.write(packet)

		response = waitForPacket(port, command, timeout)

		if response != None:
			return response

	raise Exception('LWNX command failed to receive a response.')

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
print('Running LWNX sample.')

# Make a connection to the com port.
serialPortName = '/dev/ttyUSB0'
serialPortBaudRate = 921600
port = serial.Serial(serialPortName, serialPortBaudRate, timeout = 0.1)

# Get product information.
response = executeCommand(port, 0, 0, timeout = 0.1)
print('Product: ' + readStr16(response))

# Set update rate to 78 readings per second.
print('Set update rate')
executeCommand(port, 76, 1, [8, 0])

# Set streaming to output signal data.
print('Set streaming data')
executeCommand(port, 30, 1, [2, 0, 0, 0])

print('Wait for incoming data')
# Parse incoming signal data packets.
while True:
	response = waitForPacket(port, 39)
	
	if response != None:
		currentDistance, signalWidth = readSignalData(response)
		print('Distance: {} m Width: {} m'.format(currentDistance, signalWidth))