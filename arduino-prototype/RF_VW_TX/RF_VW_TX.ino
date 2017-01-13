// transmitter based on the original virtualwire transmitter example from Mike McCauley
// uses 433 MHz modules from IC Station: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// power with 5V. 
// connect data pin to pin 12 (or whatever you set)
// I didn't need an antenna to get this going, but that's with devices 20cm from eachother.
// 
#include "VirtualWire.h"

const int TX_PIN = 12;
void setup()
{
    Serial.begin(9600);    // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_tx_pin(TX_PIN);
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec
    Serial.println("setup complete");
}

void loop()
{
    const char *msg = "Hello World";
    digitalWrite(13, true); // Flash a light to show transmitting
    Serial.print("Transmitting: "); Serial.println(msg);
    delay(150);
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    Serial.println("Transmit Complete");
    digitalWrite(13, false);
    delay(5000);
}
