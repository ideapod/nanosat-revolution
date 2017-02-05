// transmitter based on the original from the radiohead example simple ASK transmitter code
// uses 433 MHz modules from IC Station: http://www.icstation.com/433mhz-transmitter-receiver-arduino-project-p-1402.html
// power with 5V. 
// connect data pin to pin 12 (or whatever you set)
// I didn't need an antenna to get this going, but that's with devices 20cm from eachother.
// see http://www.airspayce.com/mikem/arduino/RadioHead/classRH__ASK.html for more on how to use radio head and the ASK driver.
// default set up is RH_ASK (uint16_t speed=2000, uint8_t rxPin=11, uint8_t txPin=12, uint8_t pttPin=10, bool pttInverted=false)
// so, 2000 bits per second, pin 11 for rx, pin 12 for tx, and ptt inverted.
//
// this sketch also assumes an MPU9250 connected via I2C (pins A4, A5)
// 

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include "Wire.h"

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C
 
#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18
 
#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

#define   MPU9250_REG_GYRO_CONFIG   27
#define   MPU9250_REG_ACCEL_CONFIG   28
#define   MPU9250_REG_INT_PIN_CFG 55
#define   BYPASS_EN 0x02

#define   MPU9250_AK8963_MAG_CNTL 0x0A
#define   MPU9250_AK8963_MAG_CNTL_SINGLE_MEASURE 0x01
 
#include <I2CHelper.h>

// radiohead declaration of ASK driver.
RH_ASK driver; // by default uses 2000 bits per second, pin 11 for rx, pin 12 for tx, and ptt inverted.

struct mpu_data {
  int16_t accel_x;
  int16_t accel_y;
  int16_t accel_z;

  int16_t gyro_x;
  int16_t gyro_y;
  int16_t gyro_z;

  int16_t mag_x;
  int16_t mag_y;
  int16_t mag_z;

};

// Initializations
void setup()
{
  // Arduino initializations
  Wire.begin();
  Serial.begin(9600);

  // init 9250
  initMPU9250();

  // init comms
  initComms();
}


long int cpt=0;
// Main loop, read and display data
void loop()
{
  // wait for next measure from the MPU9250
  if (qryMagMeasure()) 
  {
    // _______________
    // ::: Counter :::
    Serial.print ("Counter "); 
    // Display data counter
    Serial.print (cpt++,DEC);
    Serial.print ("\n");

    bool retVal = false;
    mpu_data current_measure = { 0,0,0,0,0,0,0,0,0};

    sendData("Mag reading follows");
    retVal = getMPU9250Measures(&current_measure);
    if (retVal == false)
    {
      Serial.println("error retrieving mpu9250 data");
    } else
    {
      showMPU9250Measure(&current_measure);
      Serial.println("Sending MPU Data");
      sendMPU9250Measure(&current_measure);
      Serial.println("MPU Data Sent");
    }
    
    delay (5000);
    // Request first magnetometer single measurement
    I2CHelper::I2CwriteByte(MAG_ADDRESS,MPU9250_AK8963_MAG_CNTL,MPU9250_AK8963_MAG_CNTL_SINGLE_MEASURE);
  }
  
  // End of line
  // Serial.println("");
  //  delay(100);    
}

bool getMPU9250Measures(mpu_data * measure)
{
  // ____________________________________
  // :::  accelerometer and gyroscope ::: 
 
  // Read accelerometer and gyroscope
  uint8_t Buf[14];
  I2CHelper::I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
  // Create 16 bits values from 8 bits data
 
  // Accelerometer
  measure->accel_x= -(Buf[0]<<8 | Buf[1]);
  measure->accel_y= -(Buf[2]<<8 | Buf[3]);
  measure->accel_z= Buf[4]<<8 | Buf[5];
 
  // Gyroscope
  measure->gyro_x= -(Buf[8]<<8 | Buf[9]);
  measure->gyro_y= -(Buf[10]<<8 | Buf[11]);
  measure->gyro_z= Buf[12]<<8 | Buf[13];

  // Read magnetometer data  
  uint8_t Mag[7];  
  I2CHelper::I2Cread(MAG_ADDRESS,0x03,7,Mag);
     
  // Create 16 bits values from 8 bits data
   
  // Magnetometer
  measure->mag_x= -(Mag[3]<< 8 | Mag[2]);
  measure->mag_y= -(Mag[1]<<8 | Mag[0]);
  measure->mag_z= -(Mag[5]<<8 | Mag[4]);

  // strange adjustment code - why?
  measure->mag_x = measure->mag_x+200;
  measure->mag_y = measure->mag_y-70;
  measure->mag_z = measure->mag_z -700;  

  return true;
}


void initMPU9250()
{
    // Configure gyroscope range
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_GYRO_CONFIG,GYRO_FULL_SCALE_2000_DPS);
  // Configure accelerometers range
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_ACCEL_CONFIG,ACC_FULL_SCALE_16_G);
  // Set by pass mode for the magnetometers
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_INT_PIN_CFG,BYPASS_EN);
 
  // Request first magnetometer single measurement
  I2CHelper::I2CwriteByte(MAG_ADDRESS,MPU9250_AK8963_MAG_CNTL,MPU9250_AK8963_MAG_CNTL_SINGLE_MEASURE);
}

void initComms()
{
  if (driver.init()) {
    Serial.println("driver initialised ok");
  } else {
    Serial.println("init failed");
  } 

}

void sendData(char *msg)
{
  Serial.print("send msg: ");
  Serial.println(msg);
  if (strlen(msg) > RH_ASK_MAX_MESSAGE_LEN)
  { // this is stupid code
    // TODO checksum
    // TODO recombine 
    // TODO investigate Datagram in manager classes - does that do all this?
    // TODO retransmit etc.
    
    Serial.println("breaking message down");
    String incomingMsg = String(msg);
    String remainderMsg = incomingMsg.substring(RH_ASK_MAX_MESSAGE_LEN +1, strlen(msg));
    Serial.print("Remainder: "); Serial.println(remainderMsg);
    char *remainder = (char *)malloc((int) incomingMsg.length());
    remainderMsg.toCharArray(remainder, incomingMsg.length());
    String pktMsg = incomingMsg.substring(0, RH_ASK_MAX_MESSAGE_LEN);
    Serial.print("Packet: "); Serial.println(pktMsg);
    char pkt[RH_ASK_MAX_MESSAGE_LEN];
    pktMsg.toCharArray(pkt,RH_ASK_MAX_MESSAGE_LEN);
    sendPacket(pkt);
    sendData(remainder);
    free(remainder);
  }
  else {
    sendPacket(msg);
  }
    
}

void sendPacket(char *msg)
{
      Serial.print("Sending:");
    Serial.print(strlen(msg));
    Serial.print(" characters: ");
    Serial.print(msg);

  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
    Serial.println(": complete");

}

void showMPU9250Measure(mpu_data *current_measure)
{
    // Display values
 
  // Accelerometer
  Serial.print("Accelerometer: \n"); 
  Serial.print("x: "); Serial.print (current_measure->accel_x,DEC); 
  Serial.print ("\t");
  Serial.print("y: "); Serial.print (current_measure->accel_y,DEC);
  Serial.print ("\t");
  Serial.print("z: "); Serial.print (current_measure->accel_z,DEC);  
  Serial.print ("\n");
 
  // Gyroscope
  Serial.print("Gyro: \n"); 
  Serial.print("x: "); Serial.print (current_measure->gyro_x,DEC); 
  Serial.print ("\t");
  Serial.print("y: "); Serial.print (current_measure->gyro_y,DEC);
  Serial.print ("\t");
  Serial.print("z: "); Serial.print (current_measure->gyro_z,DEC);  
  Serial.print ("\n");

  // Magnetometer
  Serial.print("Mag: \n"); 
  Serial.print("x: "); Serial.print (current_measure->mag_x,DEC); 
  Serial.print ("\t");
  Serial.print("y: "); Serial.print (current_measure->mag_y,DEC);
  Serial.print ("\t");
  Serial.print("z: "); Serial.print (current_measure->mag_z,DEC);  
  Serial.print ("\n");
}

void sendMPU9250Measure(mpu_data *current_measure)
{
    // Display values
 
  // Accelerometer

  String msg =String("MPU data: ");
  char buf[255];
  
  msg += "Accel: "; 
  msg += "x: "; msg += current_measure->accel_x; 
  msg += ",";
  msg += "y: "; msg += current_measure->accel_y;
  msg += ",";
  msg += "z: "; msg += current_measure->accel_z;  
 
  // Gyroscope
  msg += " Gyro: "; 
  msg += "x: "; msg += current_measure->gyro_x; 
  msg += ",";
  msg += "y: "; msg += current_measure->gyro_y;
  msg += ",";
  msg += "z: "; msg += current_measure->gyro_z;  

  // Magnetometer
  msg += " Mag: "; 
  msg += "x: "; msg += current_measure->mag_x; 
  msg += ",";
  msg += "y: "; msg += current_measure->mag_y;
  msg += ",";
  msg += "z: "; msg += current_measure->mag_z;  

  msg.toCharArray(buf,255);
  sendData (buf);
}

bool qryMagMeasure()
{
    // Read register Status 1 and wait for the DRDY: Data Ready

  uint8_t ST1;
  I2CHelper::I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  return (ST1&0x01);
}


