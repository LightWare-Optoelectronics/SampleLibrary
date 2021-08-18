//-------------------------------------------------------------------------------------------
// LightWare SF23 low power serial connection sample.
// https://lightwarelidar.com
//-------------------------------------------------------------------------------------------
// Compatible with:
// - SF23
//-------------------------------------------------------------------------------------------
// Please note that this sample needs an Arduino board that supports at least 2 hardware
// UARTs. If you are using an Uno you can use the software serial library instead of the
// Serial1 hardware UART.
//-------------------------------------------------------------------------------------------

// Calculate the CRC checksum based on data.
uint16_t lwnxCreateCrc(uint8_t* Data, uint16_t Size)
{
  uint16_t crc = 0;

  for (uint32_t i = 0; i < Size; ++i)
  {
    uint16_t code = crc >> 8;
    code ^= Data[i];
    code ^= code >> 4;
    crc = crc << 8;
    crc ^= code;
    code = code << 5;
    crc ^= code;
    code = code << 7;
    crc ^= code;
  }

  return crc;
}

void setup() {
  // Connect to the Arduino serial monitor.
  Serial.begin(115200);

  // Connect to the SF23 on Serial1.
  Serial1.begin(9600);
}

void loop() {
  uint8_t packetBuffer[8];
  int packetSize = 0;

  // Get packet of 7 bytes.
  while (packetSize != 7) {

    // Wait for incoming data.
    while (!Serial1.available());
    uint8_t c = Serial1.read();
    
    // Make sure that the first byte we receive is the header byte (0xAA).
    if (packetSize == 0 && c != 0xAA) {
      continue;      
    }
    
    // Add new data to the packet buffer.
    packetBuffer[packetSize++] = c;
  }

  // Extract data from packet.
  int distanceCm = packetBuffer[1] | (packetBuffer[2] << 8);
  int signalStrengthPercent = packetBuffer[3] | (packetBuffer[4] << 8);
  uint16_t crc = packetBuffer[5] | (packetBuffer[6] << 8);

  // Verify packet CRC.  
  uint16_t calcualtedCrc = lwnxCreateCrc(packetBuffer, 5);

  // The packet CRC was valid then distanceCm and signalStrengthPercent contain valid results.
  if (crc == calcualtedCrc) {
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.print(" cm Strength: ");    
    Serial.print(signalStrengthPercent);
    Serial.println(" %");
  }
}
