// transmitter.pde
//
// Simple example of how to use VirtualWire to transmit messages
// Implements a simplex (one-way) transmitter with an TX-C1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley
// Copyright (C) 2008 Mike McCauley
// $Id: transmitter.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include "VirtualWire.h"
#undef int
#undef abs
#undef double
#undef float
#undef round
void setup()
{
    Serial.begin(9600);    // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec
    Serial.println("setup complete");
}

void loop()
{
    const char *msg = "LMR";
    digitalWrite(13, true); // Flash a light to show transmitting
    Serial.print("Transmitting: "); Serial.println(msg);
    delay(150);
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    Serial.println("Transmit Complete");
    digitalWrite(13, false);
    delay(5000);
}
