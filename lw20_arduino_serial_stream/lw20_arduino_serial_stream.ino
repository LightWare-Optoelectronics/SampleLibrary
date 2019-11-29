//----------------------------------------------------------------------------------------------
// LightWare Optoelectronics
//----------------------------------------------------------------------------------------------
// Example for using the LW20 Serial interface with Arduino.
//----------------------------------------------------------------------------------------------

#define USER_SERIAL           Serial
#define USER_SERIAL_BAUDRATE  115200

#define LW20_SERIAL           Serial1
#define LW20_BAUDRATE         115200

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
  LW20_SERIAL.begin(LW20_BAUDRATE);

  // Run a loop that gets the streamed data.
  while (true) {
    if (lw20GetStreamResponse()) {
      float distance = getNumberFromResponse(responseData);
      USER_SERIAL.print("Distance: ");
      USER_SERIAL.println(distance);
    } else {
      USER_SERIAL.println("No streamed data within timeout");
    }
  }
}

//----------------------------------------------------------------------------------------------
// Wait up to 1 second for a streamed response.
// The response string will be held in 'responseData'.
//----------------------------------------------------------------------------------------------
bool lw20GetStreamResponse() {
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


