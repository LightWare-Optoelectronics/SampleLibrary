#------------------------------------------------------------------------------
# LightWare Raspberry Pi Python Example
#------------------------------------------------------------------------------

import time
import smbus

# Initialize the I2C Bus with I2C File 1. (Older Pis need File 0)
i2c = smbus.SMBus(1)

while True:
	
	# The reading is a 2 byte value that requires a byte swap.
	reading = i2c.read_word_data(0x55, 0)	
	reading = ((reading >> 8) | (reading << 8)) & 0xFFFF
	
	# The measurement reading is now complete.
	print reading, "cm"
	time.sleep(0.1)