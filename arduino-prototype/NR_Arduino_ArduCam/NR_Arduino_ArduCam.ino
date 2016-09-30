
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
#include "ov5642_regs.h"

// set pin 4 as the slave select for the digital pot:
const int CS = 10;



ArduCAM myCAM(OV5642,CS);

void getServiceTimingRegister()
{
  uint8_t temp;
  //Check if the ArduCAM SPI bus is OK
  Serial.print("Getting Service Interface Timing Register(hex): ");
  temp = myCAM.read_reg(ARDUCHIP_TIM);
  Serial.println(temp, HEX);
  
}

void checkSPI()
{
  uint8_t temp;
  //Check if the ArduCAM SPI bus is OK
  Serial.println("testing Arducam SPI");
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  Serial.println(temp);
  if(temp != 0x55)
  {
    Serial.println("SPI1 interface Error!");
    //while(1);
  }
  else
    Serial.println("SPI checks out ok");

  getServiceTimingRegister();
}

void checkChip()
{
  uint8_t vid,pid;
  //Check if the camera module type is OV5642
  Serial.println("Checking chip");
  
  Serial.print("OV5642_CHIPID_HIGH: "); Serial.println(OV5642_CHIPID_HIGH, HEX);
  Serial.print("OV5642_CHIPID_LOW: "); Serial.println(OV5642_CHIPID_LOW, HEX);
  Serial.println();
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if((vid != 0x56) || (pid != 0x42)) {
    Serial.print("vid (hex): "); Serial.print(vid,HEX);
    Serial.println();
    Serial.print("pid (hex): "); Serial.print(pid,HEX);
    Serial.println();
    Serial.print("vid (dec): "); Serial.print(vid,DEC);
    Serial.println();
    Serial.print("pid (dec): "); Serial.print(pid,DEC);
    Serial.println();
    Serial.println("Can't find OV5642 module!");
   }
   else
    Serial.println("OV5642 detected.");
}

void setup() {
  
  
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  Serial.begin(115200);
  Serial.println("ArduCAM Start!"); 

  // set the SPI_CS as an output:
  pinMode(CS, OUTPUT);

  // initialize SPI:
  SPI.begin();
   
  checkSPI();

  checkChip();

  myCAM.write_reg(ARDUCHIP_MODE, 0x00);
  //Change to JPEG capture mode and initialize the OV5642 module	
  myCAM.set_format(BMP);
  myCAM.InitCAM();
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  
  myCAM.clear_fifo_flag();
  myCAM.write_reg(ARDUCHIP_FRAMES,0x00);
  myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  
  Serial.println("OV5642 initialized");
  Serial.println("What resolution?");
  Serial.println("0 = 320x240, 1=640x480, 2=1280x720, 3=1920x1080, 4=2048x1563, 5=2592x1944, 6=capture, 7=testSPI");
}

uint8_t processUserCommand()
{
   uint8_t temp;
   uint8_t start_capture = 0;
   
   temp = Serial.read();
    Serial.print("command hex: "); Serial.println(temp, HEX);
    switch(temp)
    {
      case 0x30:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_320x240);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x31:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_640x480);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x32:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_1280x720);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x33:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_1920x1080);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x34:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_2048x1563);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x35:
        myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        myCAM.OV5642_set_JPEG_size(OV5642_2592x1944);
        myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
        break;
      case 0x36:
        start_capture = 1;  
        Serial.println("CAM1 start single shot.");
        delay(800);
        break;
      case 0x37:
        checkSPI();
        break;  
      default:
        break;
    }
    return start_capture;
}

void startCapture()
{
        /*
      0x04 RW FIFO control Register
Bit[0]: write ‘1’ to clear FIFO write done flag
Bit[1]: write ‘1’ to start capture
Bit[4]: write ‘1’ to reset FIFO write pointer
Bit[5]: write ‘1’ to reset FIFO read pointer
*/

      Serial.println("disabling low power");
      myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//disable low power      
      Serial.println("starting capture");
      myCAM.flush_fifo();
      myCAM.clear_fifo_flag();  
      // hack - think the 5642 needs this register bit set to start capture.
      Serial.println("hack: tell 0x84 with 0x02");
      myCAM.set_bit(0x84,0x02);
      //Start capture
      myCAM.start_capture();
}

void dumpImage()
{
  uint8_t temp,temp_last;  
  int byteCount = 0;
  
  temp = 0;
  temp_last = 0;
  
  int maxBytes = myCAM.read_fifo_length();
  // if ( maxBytes > 0) {
        
    Serial.print("getting data:"); Serial.println(maxBytes);
    // while( (temp != 0xD9) && (temp_last != 0xFF) && (byteCount <= maxBytes) )
    while( (temp != 0xD9) && (temp_last != 0xFF))
    {
       temp_last = temp;
       temp = myCAM.read_fifo();
       byteCount = byteCount + 1;
       
       // Serial.print(temp,HEX);
       //every 40 characters newline
       if ((byteCount % 80) == 0) {
              // Serial.println();
              Serial.print(".");
       }

       if ((byteCount % 8000) == 0) {
              Serial.println();
       }
       
       Serial.print("temp: "); Serial.print(temp,HEX);
       Serial.print(" temp_last:"); Serial.println(temp_last,HEX);
    }
        // Serial.write(temp);
        // delayMicroseconds(10);
    Serial.print("Read finished. Number of bytes (decimal): "); Serial.println(byteCount, DEC); 
//    }
//      else 
//      {
//        Serial.println("but no data");
//      }
}


void loop() {

  uint8_t start_capture = 0;

  if (Serial.available() > 0) {
    Serial.println("serial available");
    start_capture = processUserCommand(); 
    if(start_capture == 1)
    {
      myCAM.diagnosticsOn();

      startCapture();
    }
  }
  else
    delay(100);

  if (myCAM.isCapturing() && myCAM.isCaptureComplete()== true)
  {
      Serial.println("CAM1 Capture Done!");
      dumpImage();
      myCAM.complete_capture();

  }
}
