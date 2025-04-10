#--------------------------------------------------------------------------------------------------------------
# LightWare 2025
#--------------------------------------------------------------------------------------------------------------
# Description:
#	This samples communicates with the LW20/SF20 through the LWNX protocol.
#
# Notes:
#	Requires the pySerial module.
#--------------------------------------------------------------------------------------------------------------

import time
import serial

# -----------------------------------------------------------------------------
# LWNX library functions: Helpers.
# -----------------------------------------------------------------------------
def create_crc(data):
	"""Creates a 16-bit CRC of the specified data."""

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

# -----------------------------------------------------------------------------
# LWNX library functions: Request.
# -----------------------------------------------------------------------------
class Request:
	"""Handles the creation of request packets."""

	command_id: int
	data: bytearray

	def __init__(self, command_id: int, write: bool, data: list[int] = None):
		self.data = []

		if data is None:
			data = []

		self.command_id = command_id

		payload_length = 1 + len(data)
		flags = payload_length << 6

		if write:
			flags = flags | 0x1

		packet_bytes = [0xAA, flags & 0xFF, (flags >> 8) & 0xFF, command_id]
		packet_bytes.extend(data)
		crc = create_crc(packet_bytes)
		packet_bytes.append(crc & 0xFF)
		packet_bytes.append((crc >> 8) & 0xFF)

		self.data = bytearray(packet_bytes)

def create_request_read(command_id: int) -> Request:
	return Request(command_id, False)
	
def create_request_write_int8(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 1, byteorder="little", signed=True))

def create_request_write_int16(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 2, byteorder="little", signed=True))

def create_request_write_int32(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 4, byteorder="little", signed=True))

def create_request_write_uint8(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 1, byteorder="little", signed=False))

def create_request_write_uint16(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 2, byteorder="little", signed=False))

def create_request_write_uint32(command_id: int, value: int) -> Request:
	return Request(command_id, True, int.to_bytes(value, 4, byteorder="little", signed=False))

def create_request_write_data(command_id: int, data: list[int]) -> Request:
	return Request(command_id, True, data)

# -----------------------------------------------------------------------------
# LWNX library functions: Response.
# -----------------------------------------------------------------------------
class Response:
	"""Handles the parsing of response packets."""

	command_id: int
	data: bytearray
	parse_state: int
	payload_size: int

	def __init__(self):
		self.reset()

	def reset(self):
		self.command_id = None
		self.data = []
		self.parse_state = 0
		self.payload_size = 0

	def feed(self, byte: int):
		if self.parse_state == 0:
			if byte == 0xAA:
				self.parse_state = 1
				self.data = [0xAA]

		elif self.parse_state == 1:
			self.parse_state = 2
			self.data.append(byte)

		elif self.parse_state == 2:
			self.parse_state = 3
			self.data.append(byte)
			self.payload_size = (self.data[1] | (self.data[2] << 8)) >> 6
			self.payload_size += 2

			if self.payload_size > 1019 or self.payload_size < 3:
				self.parse_state = 0

		elif self.parse_state == 3:
			self.data.append(byte)
			self.payload_size -= 1

			if self.payload_size == 0:
				self.parse_state = 0
				crc = self.data[len(self.data) - 2] | (
					self.data[len(self.data) - 1] << 8
				)
				verify_crc = create_crc(self.data[0:-2])

				if crc == verify_crc:
					self.command_id = self.data[3]
					return True

		return False

	def parse_int8(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 5 + offset], byteorder="little", signed=True)

	def parse_int16(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 6 + offset], byteorder="little", signed=True)

	def parse_int32(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 8 + offset], byteorder="little", signed=True)

	def parse_uint8(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 5 + offset], byteorder="little", signed=False)

	def parse_uint16(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 6 + offset], byteorder="little", signed=False)

	def parse_uint32(self, offset: int = 0):
		return int.from_bytes(self.data[4 + offset: 8 + offset], byteorder="little", signed=False)

	def parse_string(self, offset: int = 0):
		str16 = ""
		for i in range(0, 16):
			if self.data[4 + i + offset] == 0:
				break
			else:
				str16 += chr(self.data[4 + i + offset])

		return str16

	def parse_data(self, size, offset: int = 0):
		return self.data[4 + offset: 4 + size + offset]

# -----------------------------------------------------------------------------
# LWNX library functions: Send & receive.
# -----------------------------------------------------------------------------
def wait_for_next_response(port: serial, expected_command_id: int, timeout: float = 1) -> Response | None:
	end_time = time.time() + timeout
	response = Response()

	while True:
		if port.in_waiting == 0:
			if time.time() >= end_time:
				return None
			
		pr = port.read(1)

		if len(pr) == 0:
			continue
		
		byte = ord(pr)

		if response.feed(byte):
			if response.command_id == expected_command_id:
				return response
			else:
				response.reset()

def send_request_get_response(port: serial, request: Request, timeout: float = 1) -> Response:
	retries = 4

	while retries > 0:
		retries -= 1
		port.write(request.data)

		response = wait_for_next_response(port, request.command_id, timeout)

		if response is not None:
			return response

	raise TimeoutError("Request failed to get a response.")

#--------------------------------------------------------------------------------------------------------------
# Main application.
#--------------------------------------------------------------------------------------------------------------
def main():
	print('Running LW20/SF20 LWNX sample.')

	# Using the commands at https://support.lightware.co.za/sf20/#/commands

	# Make a connection to the com port.
	# NOTE: Make sure to change the port to the one your device is connected to.
	# The default baud rate for the LW20/SF20 is 115200.
	serial_port_path = '\\\\.\\COM8'
	serial_port_baudrate = 115200
	port = serial.Serial(serial_port_path, serial_port_baudrate, timeout = 0.1)

	# Get product name (0. Product name).
	# NOTE: This command also switches the device into LWNX mode.
	response = send_request_get_response(port, create_request_read(0))
	print('Product: ' + response.parse_string())

	# Set update rate (93. Measurement mode) to 48 readings per second (1).
	print('Set update rate')
	send_request_get_response(port, create_request_write_uint8(93, 1))

	# Set distance data (27. Distance output) to include First return median (2) and First return strength (4).
	print('Set distance data options')
	distance_flags = (1 << 2) | (1 << 4)
	send_request_get_response(port, create_request_write_uint32(27, distance_flags))

	# Set streaming (30. Stream) to output distance data (5).
	print('Set streaming data')
	send_request_get_response(port, create_request_write_uint32(30, 5))

	print('Wait for incoming data')
	# Parse incoming signal data packets (44. Distance data).
	while True:
		response = wait_for_next_response(port, 44)

		if response is not None:
			# Parse the signal data packet (44. Distance data).
			# NOTE: The reponse packet will only include data as specified in the distance_flags.
			first_return_median = response.parse_int16(0) / 100.0
			first_return_strength = response.parse_int16(2)
			print(f'Distance: {first_return_median} m Strength: {first_return_strength} %')

# Run main application
main()
