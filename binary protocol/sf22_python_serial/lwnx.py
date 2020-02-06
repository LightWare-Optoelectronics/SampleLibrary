#--------------------------------------------------------------------------------------------------------------
# LightWare 2019
#--------------------------------------------------------------------------------------------------------------
# Description:
#   This samples communicates with the SF22.
#
# Notes:
# 	Requires the pySerial module.
#--------------------------------------------------------------------------------------------------------------

import time
import serial

#--------------------------------------------------------------------------------------------------------------
# LWNX basic packet library functions.
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
# LWNX packet extraction functions.
#--------------------------------------------------------------------------------------------------------------

# Interpret packet response as a 16 byte string.
def getStr16(packetData):
	result = ''
	for i in range(0, 16):
		if packetData[4 + i] == 0:
			break
		else:
			result += chr(packetData[4 + i])

	return result

# Interpret packet response as a UInt32.
def getUInt32(packetData):
	result = packetData[4 + 0] << 0
	result |= packetData[4 + 1] << 8
	result |= packetData[4 + 2] << 16
	result |= packetData[4 + 3] << 24

	return result

# Interpret packet response as a firmware version.
def getFirmwareVersion(packetData):
	result = str(packetData[4 + 2]) + '.' + str(packetData[4 + 1]) + '.' + str(packetData[4 + 0])

	return result

# Interpret packet response as signal data.
def getSignalData(packetData):
	firstDistance = packetData[4 + 0] << 0
	firstDistance |= packetData[4 + 1] << 8
	firstDistance /= 100.0

	firstStrength = packetData[6 + 0] << 0
	firstStrength |= packetData[6 + 1] << 8
	
	lastDistance = packetData[8 + 0] << 0
	lastDistance |= packetData[8 + 1] << 8
	lastDistance /= 100.0

	lastStrength = packetData[10 + 0] << 0
	lastStrength |= packetData[10 + 1] << 8

	noise = packetData[12 + 0] << 0
	noise |= packetData[12 + 1] << 8

	temperature = packetData[14 + 0] << 0
	temperature |= packetData[14 + 1] << 8
	temperature /= 100

	return {
		'firstDistance': firstDistance,
		'firstStrength': firstStrength,
		'lastDistance': lastDistance,
		'lastStrength': lastStrength,
		'noise': noise,
		'temperature': temperature,
	}

#--------------------------------------------------------------------------------------------------------------
# SF22 API.
#--------------------------------------------------------------------------------------------------------------
# Get product information.
def sf22GetProductInformation(port):
	return { 
		'hardwareName' : getStr16(executeCommand(port, 0, 0)),
		'hardwareVersion' : getUInt32(executeCommand(port, 1, 0)),
		'firmwareVersion' : getFirmwareVersion(executeCommand(port, 2, 0)),
		'serialNumber' : getStr16(executeCommand(port, 3, 0))
	}

def sf22SetUpdateRate(port, rate):
	if rate < 1:
		rate = 1
	elif rate > 300:
		rate = 300

	executeCommand(port, 87, 1, [rate & 0xFF, rate >> 8])

def sf22StartDistanceStreaming(port):
	executeCommand(port, 27, 1, [246, 0, 0, 0])
	executeCommand(port, 30, 1, [13, 0, 0, 0])

def sf22WaitForMeasurement(port):
	response = waitForPacket(port, 204, 2)

	if response != None:
		return getSignalData(response)

	return None

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
print('Running SF22 sample.')

# Make a connection to the com port.
serialPortName = '/dev/ttyACM0'
serialPortBaudRate = 921600
port = serial.Serial(serialPortName, serialPortBaudRate, timeout = 0.5)

productInfo = sf22GetProductInformation(port)
print('Hardware name: ' + productInfo['hardwareName'])
print('Hardware version: ' + str(productInfo['hardwareVersion']))
print('Firmware version: ' + productInfo['firmwareVersion'])
print('Serial number: ' + productInfo['serialNumber'])

print('Set update rate')
sf22SetUpdateRate(port, 100)

print('Start data streaming')
sf22StartDistanceStreaming(port)

print('Read incoming distance data')
while True:
	measurementInfo = sf22WaitForMeasurement(port)

	if measurementInfo != None:
		print('First signal: {} m {} % - Last signal: {} m {} % - Noise: {} - Temperature: {} degrees'.format(
			measurementInfo['firstDistance'],
			measurementInfo['firstStrength'],
			measurementInfo['lastDistance'],
			measurementInfo['lastStrength'],
			measurementInfo['noise'],
			measurementInfo['temperature'],
		))