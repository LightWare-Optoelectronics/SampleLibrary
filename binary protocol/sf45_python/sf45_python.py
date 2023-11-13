#--------------------------------------------------------------------------------------------------------------
# LightWare 2021
#--------------------------------------------------------------------------------------------------------------
# Description:
#	This samples communicates with the SF45.
#
# Notes:
#	Requires the pySerial module.
#--------------------------------------------------------------------------------------------------------------

import time
import serial

#--------------------------------------------------------------------------------------------------------------
# LWNX library functions.
#--------------------------------------------------------------------------------------------------------------
_packet_parse_state = 0
_packet_payload_size = 0
_packet_size = 0
_packet_data = []

# Create a CRC-16-CCITT 0x1021 hash of the specified data.
def create_crc(data):
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
def build_packet(command, write, data=[]):
	payload_length = 1 + len(data)
	flags = (payload_length << 6) | (write & 0x1)
	packet_bytes = [0xAA, flags & 0xFF, (flags >> 8) & 0xFF, command]
	packet_bytes.extend(data)
	crc = create_crc(packet_bytes)
	packet_bytes.append(crc & 0xFF)
	packet_bytes.append((crc >> 8) & 0xFF)

	return bytearray(packet_bytes)

# Check for packet in byte stream.
def parse_packet(byte):
	global _packet_parse_state
	global _packet_payload_size
	global _packet_size
	global _packet_data

	if _packet_parse_state == 0:
		if byte == 0xAA:
			_packet_parse_state = 1
			_packet_data = [0xAA]

	elif _packet_parse_state == 1:
		_packet_parse_state = 2
		_packet_data.append(byte)

	elif _packet_parse_state == 2:
		_packet_parse_state = 3
		_packet_data.append(byte)
		_packet_payload_size = (_packet_data[1] | (_packet_data[2] << 8)) >> 6
		_packet_payload_size += 2
		_packet_size = 3

		if _packet_payload_size > 1019:
			_packet_parse_state = 0

	elif _packet_parse_state == 3:
		_packet_data.append(byte)
		_packet_size += 1
		_packet_payload_size -= 1

		if _packet_payload_size == 0:
			_packet_parse_state = 0
			crc = _packet_data[_packet_size - 2] | (_packet_data[_packet_size - 1] << 8)
			verify_crc = create_crc(_packet_data[0:-2])
			
			if crc == verify_crc:
				return True

	return False

# Wait (up to timeout) for a packet of the specified command to be received.
def wait_for_packet(port, command, timeout=1):
	global _packet_parse_state
	global _packet_payload_size
	global _packet_size
	global _packet_data

	_packet_parse_state = 0
	_packet_data = []
	_packet_payload_size = 0
	_packet_size = 0

	end_time = time.time() + timeout

	while True:
		if time.time() >= end_time:
			return None

		c = port.read(1)

		if len(c) != 0:
			b = ord(c)
			if parse_packet(b) == True:
				if _packet_data[3] == command:
					return _packet_data
				
# Send a request packet and wait (up to timeout) for a response.
def execute_command(port, command, write, data=[], timeout=1):
	packet = build_packet(command, write, data)
	retries = 4

	while retries > 0:
		retries -= 1
		port.write(packet)

		response = wait_for_packet(port, command, timeout)

		if response != None:
			return response

	raise Exception('LWNX command failed to receive a response.')

#--------------------------------------------------------------------------------------------------------------
# SF45 API helper functions.
# NOTE: Using the SF45 commands as detailed here: https://support.lightware.co.za/sf45b/#/commands
#--------------------------------------------------------------------------------------------------------------
# Extract a 16 byte string from a string packet.
def get_str16_from_packet(packet):
	str16 = ''
	for i in range(0, 16):
		if packet[4 + i] == 0:
			break
		else:
			str16 += chr(packet[4 + i])

	return str16

def print_product_information(port):
	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/0.%20product%20name
	response = execute_command(port, 0, 0, timeout = 0.1)
	print('Product: ' + get_str16_from_packet(response))

	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/2.%20firmware%20version
	response = execute_command(port, 2, 0, timeout = 0.1)
	print('Firmware: {}.{}.{}'.format(response[6], response[5], response[4]))

	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/3.%20serial%20number
	response = execute_command(port, 3, 0, timeout = 0.1)
	print('Serial: ' + get_str16_from_packet(response))

def set_update_rate(port, value):
	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/66.%20update%20rate
	# Value can be one of:
	# 1	= 50 Hz
	# 2	= 100 Hz
	# 3	= 200 Hz
	# 4	= 400 Hz
	# 5	= 500 Hz
	# 6	= 625 Hz
	# 7	= 1000 Hz
	# 8	= 1250 Hz
	# 9	= 1538 Hz
	# 10 = 2000 Hz
	# 11 = 2500 Hz
	# 12 = 5000 Hz

	if value < 1 or value > 12:
		raise Exception('Invalid update rate value.')
	
	execute_command(port, 66, 1, [value])

def set_default_distance_output(port, use_last_return = False):
	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/27.%20distance%20output
	if use_last_return == True:
		# Configure output to have 'last return raw' and 'yaw angle'.
		execute_command(port, 27, 1, [1, 1, 0, 0])
	else:
		# Configure output to have 'first return raw' and 'yaw angle'.
		execute_command(port, 27, 1, [8, 1, 0, 0])

def set_distance_stream_enable(port, enable):
	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/30.%20stream
	if enable == True:
		execute_command(port, 30, 1, [5, 0, 0, 0])
	else:
		execute_command(port, 30, 1, [0, 0, 0, 0])

def wait_for_reading(port, timeout=1):
	# https://support.lightware.co.za/sf45b/#/command_detail/command%20descriptions/44.%20distance%20data%20in%20cm
	response = wait_for_packet(port, 44, timeout)
	
	if response == None:
		return -1, 0
	
	distance = (response[4] << 0 | response[5] << 8) / 100.0
	
	yaw_angle = response[6] << 0 | response[7] << 8
	if yaw_angle > 32000:
		yaw_angle = yaw_angle - 65535

	yaw_angle /= 100.0

	return distance, yaw_angle

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
print('Running SF45/B LWNX sample.')

# Make a connection to the serial port.
# NOTE: You will need to change the port name and baud rate to match your connected SF45.
# Common Rapsberry Pi port name: /dev/ttyACM1
serial_port_name = 'COM3'
serial_port_baudrate = 921600
sensor_port = serial.Serial(serial_port_name, serial_port_baudrate, timeout = 0.1)

# Get sensor information.
print_product_information(sensor_port)

# Configure the sensor.
# NOTE: See the set_update_rate function for values that can be used.
set_update_rate(sensor_port, 1)
set_default_distance_output(sensor_port)

# Start streaming distances.
set_distance_stream_enable(sensor_port, True)

while True:
	distance, yaw_angle = wait_for_reading(sensor_port)

	if distance != -1:
		print('{} m {} deg'.format(distance, yaw_angle))
	else:
		print('No reading streamed.')