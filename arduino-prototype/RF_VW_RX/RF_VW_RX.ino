// based on Mike McAuley's VirtualWire example code
// uses the ICStation 433 MHz modules: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// Wire 5V to the VCC Pin on the receiver.
// Wire pin 11 (or whatever you configure below) to the data pin.
// I didn't need antennas to get this and the tx sketch working, but if you put any distance, then 10cm coiled antenna.
// 

#include "VirtualWire.h" 

long countIdle = 0;
int charCount = 0;
const int RX_PIN = 11;

void setup()
{
    Serial.begin(9600);  // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_rx_pin(RX_PIN); // the device is connected to pin 11.
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec

    vw_rx_start();       // Start the receiver PLL running
    Serial.println("finished setup");
}


void loop()
{
   if (vw_have_message() != true) {
     showActivity();
   } else {
     dumpMsg();
   }
}

void dumpMsg()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  resetIdleActivity();
  
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    Serial.println();
    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    Serial.print("Got ("); Serial.print(buflen); Serial.print(") bytes:");
  
    for (i = 0; i < buflen; i++)
    {
        Serial.print((char)buf[i]);
    }
    Serial.println();
    digitalWrite(13, false);
  }
}

void resetIdleActivity()
{
  countIdle = 0;
}

void showActivity() {
  countIdle += 1;
     
  if (countIdle > 60000 ) // every few rounds, go another line.
  {
    resetIdleActivity();
    Serial.print(".");
    charCount +=1; 
    if (charCount > 40)
    {
      Serial.println(".");
      charCount = 0;
    }
  }
}

