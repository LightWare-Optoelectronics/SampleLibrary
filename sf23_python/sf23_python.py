#-------------------------------------------------------------------------------------------
# LightWare SF23 serial connection sample
# https://lightwarelidar.com
#-------------------------------------------------------------------------------------------
# Description:
#   This sample reads data from an SF23 while it is in low power mode.
#
# Notes:
# 	Requires the pySerial module.
#
#-------------------------------------------------------------------------------------------

import io, serial

packetParseState = 0
packetPayloadSize = 0
packetSize = 0
packetData = []

tempStreamPos = 0
tempStream = []

def tempRead():
	global tempStreamPos
	tempStreamPos += 1
	print("Checking pos" + str(tempStreamPos))
	return -1

def getPacket(port):
	while True:
		# c = port.read(1)
		c = tempRead()

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
print('Running SF23 sample.')

# Make a connection to the com port.
serialPortName = '/dev/ttyUSB0'
serialPortBaudRate = 9600
# port = serial.Serial(serialPortName, serialPortBaudRate, timeout=0.1)

# Clear buffer of any partial responses.
# port.readline()
port = 0

# Continuously gather distance data.
while True:

	getPacket(port)
	
	# Do what you want with the distance information here.
	print(distance)