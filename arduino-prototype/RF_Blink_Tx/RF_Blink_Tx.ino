
#define rfTransmitPin 4 // RF Transmitter pin = digital pin 4
#define ledPin 13 // Onboard LED = digital pin 13

void setup() {
  pinMode(rfTransmitPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
    digitalWrite(rfTransmitPin,HIGH); // transmit a HIGH signal
    digitalWrite(ledPin, HIGH); // turn the LED ON
    delay(2000); // wait for 1 second
    
    digitalWrite(rfTransmitPin, LOW); // transmit a LOW signal
    digitalWrite(ledPin, LOW); // turn the LED off

    delay (2000);
  
}

