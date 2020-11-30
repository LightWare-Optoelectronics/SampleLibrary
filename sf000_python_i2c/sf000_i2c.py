import time
import smbus

i2c_ch = 1

# Initialize the I2C Bus with the appropritate I2C file.
# If you are using a Raspberyy Pi then the file is 1. Older Pis need File 0.
i2c = smbus.SMBus(1)

def write():
	try:
		i2c.write_byte_data(0x66, 0x00, 0x80)
		return 1
	except IOError:
		return 0

# Establish initial communication with the SF000.
while (not write()):
	write()


while True:
	# The reading is a 2 byte value that requires a byte swap.
	reading = i2c.read_word_data(0x66, 0)   
	reading = ((reading >> 8) | (reading << 8)) & 0xFFFF
	
	# The measurement reading is now complete.
	print reading, "cm"
	time.sleep(0.1)
