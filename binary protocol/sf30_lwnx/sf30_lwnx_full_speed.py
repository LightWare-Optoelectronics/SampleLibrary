#--------------------------------------------------------------------------------------------------------------
# LightWare 2024
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
# API helper functions.
# NOTE: Using the commands as detailed here: https://support.lightware.co.za/sf30d/#/commands
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
	# https://support.lightware.co.za/sf30d/#/command_detail?id=_0-product-name
	response = execute_command(port, 0, 0, timeout = 0.1)
	print('Product: ' + get_str16_from_packet(response))

	# https://support.lightware.co.za/sf30d/#/command_detail?id=_2-firmware-version
	response = execute_command(port, 2, 0, timeout = 0.1)
	print('Firmware: {}.{}.{}'.format(response[6], response[5], response[4]))

	# https://support.lightware.co.za/sf30d/#/command_detail?id=_3-serial-number
	response = execute_command(port, 3, 0, timeout = 0.1)
	print('Serial: ' + get_str16_from_packet(response))

# Extract signal data from a signal data packet.
def read_full_speed_distance_packet(packetData):
	data_point_count = packetData[4 + 0] << 0
	print('Data Point Count: {}'.format(data_point_count))

	for i in range(0, data_point_count):
		distance = packetData[4 + 1 + (i * 2) + 0] << 0
		distance |= packetData[4 + 1 + (i * 2) + 1] << 8
		distance /= 100.0
		print('Distance: {} m'.format(distance))

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
print('Running LWNX sample.')

# Make a connection to the serial port.
# NOTE: You will need to change the port name and baud rate to match your connected device.
# Common Rapsberry Pi port name: /dev/ttyACM1
serial_port_name = '\\\\.\\COM16'
serial_port_baudrate = 921600
sensor_port = serial.Serial(serial_port_name, serial_port_baudrate, timeout = 0.1)

# Get sensor information.
print_product_information(sensor_port)

# Set return mode to last.
execute_command(sensor_port, 77, 1, [8, 0])

# Set streaming mode to 11.
execute_command(sensor_port, 30, 1, [11, 0, 0, 0])

print('Wait for incoming data')
# Parse incoming signal data packets.
while True:
	response = wait_for_packet(sensor_port, 40)
	
	if response != None:
		read_full_speed_distance_packet(response)