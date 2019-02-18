#--------------------------------------------------------------------------------------------------------------
# LightWare 2019
#--------------------------------------------------------------------------------------------------------------
# Description:
#   This sample connects to an LW20 over the serial UART hardware and communicates using the MMI protocol.
#
# Notes:
# 	Requires the pySerial module.
#--------------------------------------------------------------------------------------------------------------

import time
import serial

print('Running LW20 sample.')

# Make a connection to the com port.
serialPortName = '/dev/ttyUSB0'
serialPortBaudRate = 115200
port = serial.Serial(serialPortName, serialPortBaudRate, timeout=0.1)

# Enable serial mode by sending some characters over the serial port.
port.write('www\r\n')
# Read and ignore any unintended responses
port.readline()

# Get the product information
port.write('?\r\n')
productInfo = port.readline()
print('Product information: ' + productInfo)

while True:	
	# Get distance reading (First return, default filtering)
	port.write('LD\r\n')
	distanceStr = port.readline()
	# Convert the distance string response into a number
	distanceCM = float(distanceStr) * 100

	# Do what you want with the distance information here
	print(distanceCM)

	# Wait for 50ms before the next reading is taken
	time.sleep(0.05)