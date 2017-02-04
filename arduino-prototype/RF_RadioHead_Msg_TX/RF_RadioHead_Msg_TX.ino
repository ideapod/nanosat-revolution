// sending data reliably, from the radio example code: ask_reliable_datagram_client.pde
// uses 433 MHz modules from IC Station: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// power with 5V. 
// connect transmit module data pin to pin 12 (or whatever you set), and receive module to pin 11
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
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  manager.setTimeout(500);
  manager.setRetries(5);
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

uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void loop() 
{


  Serial.print("Sending to SERVER:");
  Serial.print(sizeof(data));
  Serial.println(": bytes");
    
  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    Serial.println("Send successfully, waiting for reply");
    getReply();
  }
  else
  {
    Serial.println("sendtoWait failed ");
    Serial.print(manager.retries());
    Serial.println(" number of retries");
  }

  Serial.println("Wait for 2 seconds before going again");
  delay(2000);
}

void getReply()
{
   // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t flags = RH_FLAGS_ACK;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from, NULL, &flags))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is ask_reliable_datagram_server running?");
    }
}

