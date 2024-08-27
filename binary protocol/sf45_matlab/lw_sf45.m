% -----------------------------------------------------------------------
% LightWare binary protocol functions.
% -----------------------------------------------------------------------

% Generate a CRC based on a uint8 array of data.
function crc = lwCreateCrc(data)
    crc = uint16(0);
    size = length(data);
    
    for i = 1:size
        code = bitshift(crc, -8);
        code = bitxor(code, uint16(data(i)));
        code = bitxor(code, bitshift(code, -4));
        crc = bitshift(crc, 8);
        crc = bitxor(crc, code);
        code = bitshift(code, 5);
        crc = bitxor(crc, code);
        code = bitshift(code, 7);
        crc = bitxor(crc, code);
    end
end

% Create a full binary packet.
% commandId: The ID of the command.
% write: 0 for a 'read' request, and 1 for a 'write' request.
% data: a uint8 array of data to send with the request.
function packet = lwCreatePacket(commandId, write, data)
    % Calculate payload length and flags
    startByte = 0xAA;
    flags = uint16(bitshift(1 + length(data), 6) + bitand(write, 1));
    
    packet = [startByte typecast(flags, "uint8") commandId data]; 

    % Calculate CRC
    crc = lwCreateCrc(packet);
    
    packet = [packet typecast(crc, "uint8")];
end

% Wait for a full binary packet to be received from the serial port.
function [payloadData, failed] = lwGetPacket(serialPort, expectedId, timeout)
    tic;
    packetState = 0;
    packetFlags = uint16(0);
    payloadSize = 0;
    failed = false;
    payloadData = [];
    
    while toc < timeout
        numBytesAvailable = serialPort.NumBytesAvailable;

        if numBytesAvailable == 0
            pause(0.01);
            continue
        end

        switch packetState
            case 0
                if numBytesAvailable >= 1
                    startByte = serialPort.read(1, "uint8");
                    if startByte == 0xAA
                        packetState = 1;
                    end
                end

            case 1
                if numBytesAvailable >= 2
                    packetFlags = uint16(serialPort.read(1, "uint16"));
                    payloadSize = bitshift(packetFlags, -6);

                    if payloadSize > 1020 || payloadSize < 1
                        % The packet is contains a payload that is too large.
                        packetState = 0;
                    else
                        packetState = 2;
                    end
                end

            case 2
                if numBytesAvailable >= payloadSize + 2
                    commandId = serialPort.read(1, "uint8");
                    payloadData = [];

                    if payloadSize > 1
                        payloadData = uint8(serialPort.read(payloadSize - 1, "uint8"));                        
                    end

                    crcData = uint16(serialPort.read(1, "uint16"));

                    packetBuffer = [0xAA typecast(packetFlags, "uint8") commandId payloadData];
                    % disp(dec2hex(packetBuffer));
                    verifyCrc = lwCreateCrc(packetBuffer);

                    if crcData ~= verifyCrc 
                        % The packet has an invalid CRC.
                        packetState = 0;
                    else                        
                        if expectedId == commandId
                            % Found a full packet with matching ID.
                            return
                        else
                            packetState = 0;
                        end
                    end
                end
        end
    end

    failed = true;
end

% This function will send a request command and wait for a respone.
% commandId: The ID of the command.
% write: 0 for a 'read' request, and 1 for a 'write' request.
% data: a uint8 array of data to send with the request.
function response = lwExecuteCommand(serialPort, commandId, write, data)
    packet = lwCreatePacket(commandId, write, data);
    attempts = 4;

    while attempts > 0
        attempts = attempts - 1;

        serialPort.write(packet, "uint8");
        % disp(dec2hex(packet));
        [response, respFailed] = lwGetPacket(serialPort, commandId, 1);

        if respFailed == false
            return;
        end
    end

    error("Failed to execute a command");
end

% -----------------------------------------------------------------------
% SF45 specific commands.
%
% NOTE: Some commands include examples of what the bytes look like for
% the 'request' and 'response' packets. The 'request' example is usually
% the same for all devices, however the 'response' example may vary 
% depending on your device.
% -----------------------------------------------------------------------

function name = sf45GetProductName(serialPort)
    % Command 0:
    % Request: 0xAA 0x40 0x00 0x00 0x70 0x9F
    % Response: 0xAA 0x40 0x04 0x00 0x53 0x46 0x34 0x35 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xD3 0x3D

    response = lwExecuteCommand(serialPort, 0, 0, []);
    name = char(response);
end

function version = sf45GetHardwareVersion(serialPort)
    % Command 1:
    % Request: 0xAA 0x40 0x00 0x01 0x51 0x8F
    % Reponse: 0xAA 0x40 0x01 0x01 0x01 0x00 0x00 0x00 0x3C 0x53

    response = lwExecuteCommand(serialPort, 1, 0, []);
    version = typecast(response, "uint32");
end

function version = sf45GetFirmwareVersion(serialPort)
    % Command 2:
    % Request: 0xAA 0x40 0x00 0x02 0x32 0xBF
    % Reponse: 0xAA 0x40 0x01 0x02 0x00 0x03 0x01 0x00 0x3B 0xA1

    response = lwExecuteCommand(serialPort, 2, 0, []);
    version = string(response(3)) + "." + string(response(2)) + "." + string(response(1));
end

function serial = sf45GetSerialNumber(serialPort)
    % Command 3:
    % Request: 0xAA 0x40 0x00 0x03 0x13 0xAF
    % Reponse: 0xAA 0x40 0x04 0x03 0x53 0x34 0x35 0x2D 0x31 0x35 0x32 0x33 0x38 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xA9 0x00

    response = lwExecuteCommand(serialPort, 3, 0, []);
    serial = char(response);
end

function sf45SetUpdateRate(serialPort, value)
    % Command 66:
    % Value can be one of:
	% 1	= 50 Hz
	% 2	= 100 Hz
	% 3	= 200 Hz
	% 4	= 400 Hz
	% 5	= 500 Hz
	% 6	= 625 Hz
	% 7	= 1000 Hz
	% 8	= 1250 Hz
	% 9	= 1538 Hz
	% 10 = 2000 Hz
	% 11 = 2500 Hz
	% 12 = 5000 Hz

    if value < 1 || value > 12
        error("Invalid update rate chosen");
    end

    lwExecuteCommand(serialPort, 66, 1, uint8(value));
end

function sf45SetDefaultDistanceOutput(serialPort, useLastReturn)
    % Command 27.

    if useLastReturn == true
        % Configure output to have 'last return raw' and 'yaw angle'.
        lwExecuteCommand(serialPort, 27, 1, [1 1 0 0]);
    else
        % Configure output to have 'first return raw' and 'yaw angle'.
        lwExecuteCommand(serialPort, 27, 1, [8 1 0 0]);
    end
end

function sf45SetDistanceStreamEnable(serialPort, enable)
	% Command 30.

    if enable == true
		lwExecuteCommand(serialPort, 30, 1, typecast(uint32(5), "uint8"));
    else
		lwExecuteCommand(serialPort, 30, 1, typecast(uint32(0), "uint8"));
    end
end

function [distance, angle] = sf45WaitForStreamedReading(serialPort, timeout)
    tic;

    while toc < timeout
        % Wait for packet response command 44.
        [response, failed] = lwGetPacket(serialPort, 44, timeout);

        if failed == false && length(response) == 4
            distance = double(typecast(response(1:2), "uint16")) / 100.0;
            angle = double(typecast(response(3:4), "int16")) / 100.0;
	        return;
        end
    end

    error("No streaming packets found");
end

% -----------------------------------------------------------------------
% Main application.
% -----------------------------------------------------------------------

% Makes sure that the serial port is not in use before we connect.
clear all;

% NOTE: Adjust the COM port and baud rate to match your device.
lidarSerial = serialport("COM3", 115200);

% Start by disabling streaming.
sf45SetDistanceStreamEnable(lidarSerial, false);

% Get product information.
productName = sf45GetProductName(lidarSerial);
disp("Product name: " + productName);

hardwareVersion = sf45GetHardwareVersion(lidarSerial);
disp("Hardware version: " + hardwareVersion);

firmwareVersion = sf45GetFirmwareVersion(lidarSerial);
disp("Firmware version: " + firmwareVersion);

serialNumber = sf45GetSerialNumber(lidarSerial);
disp("Serial number: " + serialNumber);

% Prepare SF45 for streaming.
sf45SetUpdateRate(lidarSerial, 1);
sf45SetDefaultDistanceOutput(lidarSerial, true);
sf45SetDistanceStreamEnable(lidarSerial, true);

% Read the 200 streamed distances.
% This will take 4 seconds at 50 Hz update rate.
for i = 1:50
    [distance, angle] = sf45WaitForStreamedReading(lidarSerial, 5);
    disp("Distance: " + distance + " m Angle: " + angle);
end

clear lidarSerial;