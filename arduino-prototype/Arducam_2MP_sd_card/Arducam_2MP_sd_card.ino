// from the original example by ArduCAM 
// Arducam Mini 2MP
// requires the ArduCAM library
// be sure to configure for OV2640_MINI_2MP
// 

#include <ArduCAM.h>
#include <Wire.h>
#include <SPI.h>
#include <rBase64.h>
#include "memorysaver.h"
#include <SD.h>

//This demo can only work on OV2640_MINI_2MP or OV5642_MINI_5MP or OV5642_MINI_5MP_BIT_ROTATION_FIXED
//or OV5640_MINI_5MP_PLUS or ARDUCAM_SHIELD_V2 platform.
#if !(defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined OV2640_MINI_2MP)
  #error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

#define SPI_CS_Camera 7
#define kBufferSize 80
#define kSPI_CS_card 6
#define kBufSize 200

#if defined (OV2640_MINI_2MP)
ArduCAM myCAM( OV2640, SPI_CS_Camera );
#else
ArduCAM myCAM( OV5642, SPI_CS_Camera );
#endif

int fileIndex = 0;
File newFile;
char filename[20];

void setupCard()
{ 
  while (!SD.begin(kSPI_CS_card)) {
    Serial.println("card ini failed!");
    delay(1000);
  }
  Serial.println("card init done.");
}

void setupCamera()
{
  uint8_t vid =0 , pid = 0;
  uint8_t temp = 0;


  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
   
  if (temp != 0x55){
    Serial.println("SPI1 interface Error!");
    //while(1);
  }
  else
  {
    Serial.println("SPI ok");
  }
   
 #if defined (OV2640_MINI_2MP)
     //Check if the camera module type is OV2640
     myCAM.wrSensorReg8_8(0xff, 0x01);  
     myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
     myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
     Serial.print("vid is: "); Serial.println(vid,HEX);
     Serial.print("pid is: "); Serial.println(pid,HEX);
     if ((vid != 0x26) || (pid != 0x42))
      Serial.println("Not OV2640 module!");
      if (pid == 0x41)
        Serial.println("it's an OV2641");
     else
      Serial.println("no model detected.");
  #else
   //Check if the camera module type is OV5642
    myCAM.wrSensorReg16_8(0xff, 0x01);
    myCAM.rdSensorRenx`  g16_8(OV5642_CHIPID_HIGH, &vid);
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

void setup(){

  //set the camera CS as an output:
  pinMode(SPI_CS_Camera,OUTPUT);
  pinMode(kSPI_CS_card, OUTPUT);
  
  Wire.begin();
  Serial.begin(9600);
  Serial.println("ArduCAM Start!");

  // initialize SPI:
  SPI.begin();
  delay(1000);

  setupCard();
  setupCamera();
  
  Serial.println("Setup complete");
}

void loop(){

  saveImageToFile();
  delay(10000);
}

void saveImageToFile()
{
  Serial.println("New file");
  makeNewFile(); // global newFile now has a new file created. 
  if (newFile)
  {
    Serial.println("starting capture");
    captureAndSaveToFile(); 
  }
}



void makeNewFile()
{

  
 //Construct a file name
  fileIndex = fileIndex + 1;
  sprintf(filename, "image%d.jpg", fileIndex);
  Serial.print("creating: "); Serial.println(filename);
  //Open the new file
  if (SD.exists(filename))
  {
    SD.remove(filename);
  }
  newFile = SD.open(filename, FILE_WRITE);
  if(!newFile){
    Serial.println(F("file failed"));
    return;
  }
  Serial.println("File ok");
}



void prepFifoTransfer()
{
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
}
// I'm speculating that this saves an image onboard the ArduCAM Mini's fifo buffer? 

uint32_t captureToFifo()
{
  //Flush the FIFO
  myCAM.flush_fifo();
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Start capture
  myCAM.start_capture();
  Serial.println("start Capture");
 
  while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK));
  
  uint32_t retVal = myCAM.read_fifo_length();
  Serial.print("Captured Fifo: "); Serial.print(retVal); Serial.println(" bytes");
  return retVal;
}

void fifoToBuffer(uint8_t *buf, int bufCursor)
{
  uint8_t temp = 0;
  
  temp =  SPI.transfer(0x00);
  buf[bufCursor] = temp;
}

void writeBufferToFile(uint8_t *buf,int bufSize)
{
  newFile.write(buf, bufSize);
}

void clearBuffer(uint8_t *buf, int bufSize)
{
    int i = 0;
    for (i = 0; i < bufSize; i++)
    {
      buf[i] = 0;
    }
    
}

void closeFile()
{
  newFile.close();
}



void deselectArduCam()
{
  myCAM.CS_HIGH();
}

bool checkEndJpeg(uint8_t currentByte, uint8_t lastByte)
{
  return ( (currentByte == 0xD9) && (lastByte == 0xFF) );
}

bool checkJpegHeader(uint8_t currentByte, uint8_t lastByte)
{
  return ( (currentByte == 0xD8) && (lastByte == 0xFF));
}


void writeHeaderToBuffer(uint8_t *buf)
{
  buf[0] = 0xFF;
  buf[1] = 0xD8;
}

void captureAndSaveToFile()
{
  uint32_t bytesCaptured = 0;
  int i = 0;
  uint8_t buf[kBufSize];  
  uint8_t lastByte = 0;
  uint8_t currentByte = 0;
  bool jpegHeaderReceived = false;

  bytesCaptured = captureToFifo();

  if (bytesCaptured <=0)
  {
    Serial.println("error: image too small");
    return;
  }

  prepFifoTransfer();
  Serial.println("buf to file");
  while (bytesCaptured--)
  {
    fifoToBuffer(buf,i);
    lastByte = currentByte;
    currentByte = buf[i];

    // wait for jpeg header before saving data
    if (jpegHeaderReceived == false && checkJpegHeader(currentByte,lastByte))
    {
      jpegHeaderReceived = true;
      writeHeaderToBuffer(buf);
      i = 2;
    }
    else 
    {
      i++;
    }
    
    // DEBUG - Serial.print(currentByte,HEX);
    if (checkEndJpeg(currentByte, lastByte))
    {
      Serial.print("end jpeg: ");
      Serial.print(bytesCaptured);
      Serial.println(" remaining bytes");
      break;
    }
    
    if (i == kBufSize) // buffer full
    {
      Serial.println();
      Serial.print("saving buf, bytes left:"); Serial.println(bytesCaptured);
      deselectArduCam();
      
      writeBufferToFile(buf,i);
      i = 0;
      clearBuffer(buf,kBufSize); 
      prepFifoTransfer();
    }
  }
  
  if (i < kBufSize)
  {
    Serial.println();
    Serial.println("saving last buffer ");
    deselectArduCam();
    writeBufferToFile(buf,i);
    i = 0;
  }
  
  closeFile();
  Serial.println("closed file");
}


