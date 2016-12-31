// Simple example of how to use VirtualWire to receive messages
// Implements a simplex (one-way) receiver with an Rx-B1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley
// Copyright (C) 2008 Mike McCauley
// $Id: receiver.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include "VirtualWire.h" 
#undef int
#undef abs
#undef double
#undef float
#undef round
void setup()
{
    Serial.begin(9600);  // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec

    vw_rx_start();       // Start the receiver PLL running
    Serial.println("finished setup");
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

  Serial.print("<");
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
      int i;

      digitalWrite(13, true); // Flash a light to show received good message
      // Message with a good checksum received, dump it.
      Serial.print("Got ("); Serial.print(buflen); Serial.print("):");
  
      for (i = 0; i < buflen; i++)
      {
          Serial.print((char)buf[i]);
          Serial.print(" ");
      }
      Serial.println("");
      digitalWrite(13, false);
    }
    Serial.print(">");
}
