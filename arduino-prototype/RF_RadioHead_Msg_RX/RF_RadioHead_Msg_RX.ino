// receiving data reliably, from the radio example code: ask_reliable_datagram_server.pde
// uses 433 MHz modules from IC Station: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// power with 5V. 
// connect transmit module data pin to pin 12 (or whatever you set), and receiver to pin 11.
// I didn't need an antenna to get this going, but that's with devices 20cm from eachother.
// see http://www.airspayce.com/mikem/arduino/RadioHead/classRH__ASK.html for more on how to use radio head and the ASK driver.
// default set up is RH_ASK (uint16_t speed=2000, uint8_t rxPin=11, uint8_t txPin=12, uint8_t pttPin=10, bool pttInverted=false)
// so, 2000 bits per second, pin 11 for rx, pin 12 for tx, and ptt inverted.

#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_ASK driver;
// RH_ASK driver(2000, 2, 4, 5); // ESP8266: do not use pin 11

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  manager.setTimeout(500);
  manager.setRetries(6);
  manager.setHeaderFlags(RH_FLAGS_ACK);
  if (manager.init())
  {
    Serial.println("initialised ok");
  }
  else 
  {
    Serial.println("init failed");
  }
}

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop()
{
  receiveMessage();
  
  /* 
  if (manager.available())
  { 
    
  }
  */
}

void receiveMessage()
{
  // Wait for a message addressed to us from the client
  uint8_t len = sizeof(buf);
  uint8_t from;
  uint8_t flags = RH_FLAGS_ACK;


  // TODO - set ack flage here? 
  if (manager.recvfromAck(buf, &len, &from, NULL, &flags))
  {
    Serial.print("my address: ");
    Serial.print(manager.thisAddress());
    Serial.print(" got message from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char*)buf);

    sendResponse(from);
  }
}

void sendResponse(uint8_t from)
{
    // Send a reply back to the originator client
    Serial.print("Replying with: ");
    Serial.print(sizeof(data));
    Serial.println(": bytes of data");
    if (manager.sendtoWait(data, sizeof(data), from))
    {
      Serial.println("response Successful");
    }
    else {
      Serial.println("sending response failed");
    }

}

