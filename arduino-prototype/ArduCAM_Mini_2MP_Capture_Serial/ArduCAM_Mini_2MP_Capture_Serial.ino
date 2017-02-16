// ArduCAM demo (C)2016 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules, and can run on any Arduino platform.
//
// This demo was made for Omnivision 2MP/5MP sensor.
// It will run the ArduCAM ESP8266 5MP as a real 2MP digital camera, provide both JPEG capture.
// The demo sketch will do the following tasks:
// 1. Set the sensor to JPEG mode.
// 2. Capture and buffer the image to FIFO every 5 seconds 
// HACK - removed this 3. Store the image to Micro SD/TF card with JPEG format in sequential.
// HACK - added this: 3. output the data to serial 
// 4. Resolution can be changed by myCAM.set_JPEG_size() function.
// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM 2MP/5MP shield
// and use Arduino IDE 1.5.2 compiler or above
#include <ArduCAM.h>
#include <Wire.h>
#include <SPI.h>
#include <rBase64.h>
#include "memorysaver.h"

//This demo can only work on OV2640_MINI_2MP or OV5642_MINI_5MP or OV5642_MINI_5MP_BIT_ROTATION_FIXED
//or OV5640_MINI_5MP_PLUS or ARDUCAM_SHIELD_V2 platform.
#if !(defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined OV2640_MINI_2MP)
  #error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

const int SPI_CS = 7;
const int kBufferSize = 80;

#if defined (OV2640_MINI_2MP)
ArduCAM myCAM( OV2640, SPI_CS );
#else
ArduCAM myCAM( OV5642, SPI_CS );
#endif

void captureToFifo()
{
  //Flush the FIFO
  myCAM.flush_fifo();
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Start capture
  myCAM.start_capture();
  Serial.println("start Capture");
 
  while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  
  Serial.println("Captured to Fifo"); 
}



void myCAMSaveToSerial(){
  char str[8];
  byte buf[kBufferSize];
  char b64buf[kBufferSize * 3];
  static int i = 0;
  static int k = 0;
  static int n = 0;
  uint8_t temp, temp_last;
  int maxBytes = 0; 
  
  captureToFifo(); 

  maxBytes = myCAM.read_fifo_length();
  Serial.print("Retrieving data size: "); Serial.print(maxBytes); 
  
  i = 0;
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  temp=SPI.transfer(0x00);

  Serial.print("Received byte on SPI(hex): "); Serial.println(temp,HEX);
  //Read JPEG data from FIFO
  while ( (temp !=0xD9) | (temp_last !=0xFF)){
    temp_last = temp;
    temp = SPI.transfer(0x00);
    Serial.print(temp,HEX); Serial.print(" ");
    i++;

    if ((i % 20) == 0) {
      Serial.println(".");
    
      //encode buffers worth of bytes 
/*       myCAM.CS_HIGH();
      buf[i++] = temp;     
      Serial.print("About to encode: "); Serial.print(i); Serial.println(" bytes of data");
      int outlen = rbase64_encode(b64buf, (char *)buf, i);
      Serial.print("encoded buffer: "); Serial.print(outlen); Serial.println(" bytes converted"); 
      Serial.print ("buffer worth (base64): "); Serial.println(b64buf);
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      i = 0;
      */
      
     }
  }
  Serial.println(". ");
  Serial.println("Capture to Serial done");
  delay(0);  
 }

void setup(){
  uint8_t vid =0 , pid = 0;
  uint8_t temp = 0;

  Wire.begin();
  Serial.begin(9600);
  Serial.println("ArduCAM Start!");

  //set the CS as an output:
  pinMode(SPI_CS,OUTPUT);
  pinMode(6,OUTPUT);
  digitalWrite(6,HIGH);
  
  // initialize SPI:
  SPI.begin();
  delay(1000);
  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
   
  if (temp != 0x55){
    Serial.println("SPI1 interface Error!");
    //while(1);
  }
  else
  {
    Serial.println("SPI checks out ok");
  }
   
 #if defined (OV2640_MINI_2MP)
     //Check if the camera module type is OV2640
     myCAM.wrSensorReg8_8(0xff, 0x01);  
     myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
     myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
     Serial.print("vid is: "); Serial.println(vid,HEX);
     Serial.print("pid is: "); Serial.println(pid,HEX);
     if ((vid != 0x26) || (pid != 0x42))
      Serial.println("Can't find OV2640 module!");
      if (pid == 0x41)
        Serial.println("but it's an OV2641");
     else
      Serial.println("OV2640 detected.");
  #else
   //Check if the camera module type is OV5642
    myCAM.wrSensorReg16_8(0xff, 0x01);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
     if((vid != 0x56) || (pid != 0x42))
     Serial.println("Can't find OV5642 module!");
     else
     Serial.println("OV5642 detected.");
  #endif
   myCAM.set_format(JPEG);
   myCAM.InitCAM();
 #if defined (OV2640_MINI_2MP)
   myCAM.OV2640_set_JPEG_size(OV2640_320x240);
  #else
   myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);   //VSYNC is active HIGH
   myCAM.OV5642_set_JPEG_size(OV5642_320x240);
 #endif
  delay(1000);
}

void loop(){
  myCAMSaveToSerial();
  delay(10000);
}


