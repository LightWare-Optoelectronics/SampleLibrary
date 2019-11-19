//-------------------------------------------------------------------------------------------
// LightWare LW20/SF20 Arduino LWNX serial connection sample.
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// This sample connects to the LW20/SF20 with the advanced binary protocol.
// The Arduino Uno only has a single hardware serial port, and therefore this sample
// has been modified to use a software serial port that can be used on custom
// defined pins.
//-------------------------------------------------------------------------------------------

#include <SoftwareSerial.h>
SoftwareSerial LW20_SERIAL(2, 3); // RX, TX

#define USER_SERIAL           Serial
#define USER_SERIAL_BAUDRATE  115200

#define LW20_BAUDRATE         38400

// LW20 response string data.
char responseData[40];
int responseDataSize = 0;

//----------------------------------------------------------------------------------------------
// Application setup.
//----------------------------------------------------------------------------------------------
void setup() {
  // Connect to the serial console.
  USER_SERIAL.begin(115200);
}

//----------------------------------------------------------------------------------------------
// Application main loop.
//----------------------------------------------------------------------------------------------
void loop() {
  USER_SERIAL.println("LW20 sample code started");
  
  // Connect to the LW20.
  lw20Connect();

  // Show product information to show a valid connection is established.
  lw20PrintProductInfo();

  // Run a loop that asks for distance every 25ms.
  while (true) {
    float distance = lw20GetDistance();
    float strength = lw20GetStrength();
    
    USER_SERIAL.print("Distance: ");
    USER_SERIAL.print(distance);
    USER_SERIAL.print(" m Strength: ");
    USER_SERIAL.print(strength);
    USER_SERIAL.println(" %");
    
    delay(25);
  }
}

//----------------------------------------------------------------------------------------------
// Conenct to the LW20 and enable serial mode.
//----------------------------------------------------------------------------------------------
void lw20Connect() {
  LW20_SERIAL.begin(LW20_BAUDRATE);

  // Send some characters to make sure the LW20 switches to serial mode.
  LW20_SERIAL.print('www\r\n');
}

//----------------------------------------------------------------------------------------------
// Send a command string and wait for a response. Returns true if a response was received.
// The response string will be held in 'responseData'.
//----------------------------------------------------------------------------------------------
bool lw20SendCommand(const char* CommandStr) {
  // Make sure there is no unwanted data left in the buffer.
  while (LW20_SERIAL.available()) {
    LW20_SERIAL.read();
  }

  // Send the command.
  LW20_SERIAL.print(CommandStr);

  // Only have 1 second timeout.
  unsigned long timeoutTime = millis() + 1000;

  responseDataSize = 0;
  
  // Wait for the full response.
  while (millis() < timeoutTime) {
    if (LW20_SERIAL.available()) {
      int c = LW20_SERIAL.read();
      
      if (c == '\n') {
        responseData[responseDataSize] = 0;
        return true;
      }
      else if (c != '\r') {
        if (responseDataSize == sizeof(responseData) - 1) {
          responseDataSize = 0;
        }
        
        responseData[responseDataSize++] = (char)c;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------
// Gets a number from the response string.
// Most commands have a single number that needs to be extracted from the response. That number
// occurs after the ':' character.
//----------------------------------------------------------------------------------------------
float getNumberFromResponse(char* ResponseStr) {
  // Find the ':' character.
  int index = 0;
  
  while (true) {    
    if (ResponseStr[index] == 0)
      break;
      
    if (ResponseStr[index] == ':') {
      return atof(ResponseStr + index + 1);
    }

    ++index;
  }

  return 0.0f;
}

//----------------------------------------------------------------------------------------------
// Display the product info response.
//----------------------------------------------------------------------------------------------
void lw20PrintProductInfo() {  
  if (lw20SendCommand("?\r\n")) {
    USER_SERIAL.print("Got product response: ");
    USER_SERIAL.println(responseData);
  } else {
    USER_SERIAL.println("Could not get product response");
  }
}

//----------------------------------------------------------------------------------------------
// Get the current distance.
//----------------------------------------------------------------------------------------------
float lw20GetDistance() {
  // NOTE: LDL = median distance to last return.
  if (lw20SendCommand("?ldl\r\n"))
    return getNumberFromResponse(responseData);

  return 0.0f;
}

//----------------------------------------------------------------------------------------------
// Get the signal strength.
//----------------------------------------------------------------------------------------------
float lw20GetStrength() {
  if (lw20SendCommand("?lh\r\n"))
    return getNumberFromResponse(responseData);

  return 0.0f;
}
