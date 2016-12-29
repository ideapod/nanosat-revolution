

#define rfReceivePin 0 // RF Receiver pin = Analog pin 0
#define ledPin 13 // Onboard LED = digital pin 13

unsigned int data = 0; // variabled used to store received data
const unsigned int upperThreshold = 140; // upper threshold value
const unsigned int lowerThreshold = 50; // lower threshold value

void setup() {
  pinMode (ledPin, OUTPUT);
  Serial.begin (9600);
}

void loop(){
  data = analogRead(rfReceivePin); // listen for data on analog pin 0
  Serial.print("rxvalue: "); Serial.println(data);
  
  if (data>upperThreshold) {
    digitalWrite(ledPin, LOW); // if a low signal is received turn LED off
    Serial.println("data high");
  }

  if (data<lowerThreshold) {
    digitalWrite(ledPin, HIGH); // if a high signal is received, run LED on
    Serial.println("data low");
  }

  delay(300);
}

