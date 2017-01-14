// receiver based on the original from the radiohead example simple ASK receiver code
// uses 433 MHz modules from IC Station: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// power with 5V. 
// connect data pin to pin 12 (or whatever you set)
// I didn't need an antenna to get this going, but that's with devices 20cm from eachother.
// see http://www.airspayce.com/mikem/arduino/RadioHead/classRH__ASK.html for more on how to use radio head and the ASK driver.
// default set up is RH_ASK (uint16_t speed=2000, uint8_t rxPin=11, uint8_t txPin=12, uint8_t pttPin=10, bool pttInverted=false)
// so, 2000 bits per second, pin 11 for rx, pin 12 for tx, and ptt inverted.
// 

#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver; // by default uses 2000 bits per second, pin 11 for rx, pin 12 for tx, and ptt inverted.
// RH_ASK driver(2000, 2, 4, 5); // ESP8266: do not use pin 11

void setup()
{
    Serial.begin(9600);  // Debugging only
    if (driver.init()) {
      Serial.println("driver initialised ok");
    } else {
      Serial.println("init failed");
    }
}

void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      
      // Message with a good checksum received, dump it.
      // driver.printBuffer("Got:", buf, buflen);
      dumpMsg(buf, buflen);
    }
}

void dumpMsg(uint8_t *buf, uint8_t buflen)
{
    int i = 0;
    Serial.print("Got ("); Serial.print(buflen); Serial.print(") bytes:");
  
    for (i = 0; i < buflen; i++)
    {
        Serial.print((char)buf[i]);
    }
    Serial.println();
}

