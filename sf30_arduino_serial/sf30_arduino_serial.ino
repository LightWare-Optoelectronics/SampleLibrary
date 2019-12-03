//-------------------------------------------------------------------------------------------
// LightWare Arduino SF30 serial connection sample.
// https://lightware.co.za
//-------------------------------------------------------------------------------------------
// This sample shows how to read serial data from the SF30.
//-------------------------------------------------------------------------------------------

// NOTE: The SF30 needs to be configured in the following way for the sample to work:
// Output type: Distance over serial
// Serial port baud rate: 115200
// Serial port output rate: Anything below 5000. We recommend starting out at 39 readings per second and working up from there.

// NOTE: This sample is using an Arduino MEGA 2560 which means there is a dedicated hardware serial port to connect the SF30 to.

void setup() {
  // Serial monitor connection on Serial.
  Serial.begin(115200);
  Serial.println("Sample starting...");

  // SF30 serial connection on Serial 1.
  Serial1.begin(115200);
}

int byteState = 0;
int byteH = 0;

void loop() {
  int c = Serial1.read();

  if (c != -1) {
    if (c & 0x80) {
      byteState = 1;
      byteH = c & 0x7F;
    } else {
      if (byteState) {
        byteState = 0;
        int distance = (byteH << 7) | c;
  
        if (distance == 16000) {
          Serial.println("Lost signal");
        } else {
          Serial.print(distance);
          Serial.println(" cm");
        }
      }
    }
  }
}
