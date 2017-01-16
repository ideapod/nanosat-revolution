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
 
// Initializations
void setup()
{
  // Arduino initializations
  Wire.begin();
  Serial.begin(115200);
 
  // Configure gyroscope range
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_GYRO_CONFIG,GYRO_FULL_SCALE_2000_DPS);
  // Configure accelerometers range
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_ACCEL_CONFIG,ACC_FULL_SCALE_16_G);
  // Set by pass mode for the magnetometers
  I2CHelper::I2CwriteByte(MPU9250_ADDRESS,MPU9250_REG_INT_PIN_CFG,BYPASS_EN);
 
  // Request first magnetometer single measurement
  I2CHelper::I2CwriteByte(MAG_ADDRESS,MPU9250_AK8963_MAG_CNTL,MPU9250_AK8963_MAG_CNTL_SINGLE_MEASURE);
 
 
}


void showAccelGyro()
{
  // ____________________________________
  // :::  accelerometer and gyroscope ::: 
 
  // Read accelerometer and gyroscope
  uint8_t Buf[14];
  I2CHelper::I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
 
 
  // Create 16 bits values from 8 bits data
 
  // Accelerometer
  int16_t ax=-(Buf[0]<<8 | Buf[1]);
  int16_t ay=-(Buf[2]<<8 | Buf[3]);
  int16_t az=Buf[4]<<8 | Buf[5];
 
  // Gyroscope
  int16_t gx=-(Buf[8]<<8 | Buf[9]);
  int16_t gy=-(Buf[10]<<8 | Buf[11]);
  int16_t gz=Buf[12]<<8 | Buf[13];
 
    // Display values
 
  // Accelerometer
  Serial.print("Accelerometer: \n"); 
  Serial.print("x: "); Serial.print (ax,DEC); 
  Serial.print ("\t");
  Serial.print("y: "); Serial.print (ay,DEC);
  Serial.print ("\t");
  Serial.print("z: "); Serial.print (az,DEC);  
  Serial.print ("\n");
 
  // Gyroscope
  Serial.print("Gyro: \n"); 
  Serial.print("x: "); Serial.print (gx,DEC); 
  Serial.print ("\t");
  Serial.print("y: "); Serial.print (gy,DEC);
  Serial.print ("\t");
  Serial.print("z: "); Serial.print (gz,DEC);  
  Serial.print ("\n");
}

bool qryMagMeasure()
{
    // Read register Status 1 and wait for the DRDY: Data Ready

  uint8_t ST1;
  I2CHelper::I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  return (ST1&0x01);
}

void showMagMeasure()
{
    // Read magnetometer data  
    uint8_t Mag[7];  
    I2CHelper::I2Cread(MAG_ADDRESS,0x03,7,Mag);
   
   
    // Create 16 bits values from 8 bits data
   
    // Magnetometer
    int16_t mx=-(Mag[3]<<
    8 | Mag[2]);
    int16_t my=-(Mag[1]<<8 | Mag[0]);
    int16_t mz=-(Mag[5]<<8 | Mag[4]);
   
   
    // Magnetometer
    Serial.print("Magnetometer: \n"); 
    Serial.print("x: "); Serial.print (mx+200,DEC); 
    Serial.print ("\t");
    Serial.print("y: "); Serial.print (my-70,DEC);
    Serial.print ("\t");
    Serial.print("z: "); Serial.print (mz-700,DEC);  
    Serial.println ("");
}
 
long int cpt=0;
// Main loop, read and display data
void loop()
{
  if (qryMagMeasure()) 
  {
    // _______________
    // ::: Counter :::
    Serial.print ("Counter "); 
    // Display data counter
    Serial.print (cpt++,DEC);
    Serial.print ("\n");
  
    showAccelGyro();
    showMagMeasure(); 
    delay (2000);
    // Request first magnetometer single measurement
    I2CHelper::I2CwriteByte(MAG_ADDRESS,MPU9250_AK8963_MAG_CNTL,MPU9250_AK8963_MAG_CNTL_SINGLE_MEASURE);
    
  }
  
  // End of line
  // Serial.println("");
  //  delay(100);    
}
