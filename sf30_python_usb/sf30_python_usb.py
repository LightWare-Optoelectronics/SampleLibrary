#-------------------------------------------------------------------------------------------
# LightWare SF30 USB serial connection sample
# https://lightware.co.za
#-------------------------------------------------------------------------------------------
# Description:
#   This sample reads data from an SF30 connected over USB.
#
# Notes:
# 	Requires the pySerial module.
#
# SF30 configuration:
#	'Active data port' must be 'USB distance in m'.
#	'USB port update rate' will determine readings per second.
#
#-------------------------------------------------------------------------------------------

import serial

print('Running SF30 sample.')

# Make a connection to the com port. USB0 is the first default port assigned to USB serial devices.
serialPortName = '/dev/ttyUSB0'
serialPortBaudRate = 115200
port = serial.Serial(serialPortName, serialPortBaudRate, timeout=0.1)

# Clear buffer of any partial responses.
port.readline()

# Continuously gather distance data.
while True:	
	# Each reading is contained on a single line.
	distanceStr = port.readline()
	
	# Convert the string to a numeric distance value.
	try:
		splitStr = distanceStr.split(" ")
		distance = float(splitStr[0])
	except ValueError:
		# It is possible that the SF30 does not get a valid signal, we represent this case as a -1.0m.
		distance = -1.0		

	# Do what you want with the distance information here.
	print(distance)