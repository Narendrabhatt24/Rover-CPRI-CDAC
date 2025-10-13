#include "I2C_Modules.h"

Adafruit_MCP4725 right_dac;    //DAC For right side motor 
Adafruit_MCP4725 left_dac;     //DAC For left side motor

void init_I2C_modules()
{
  Wire.begin(); 
  right_dac.begin(0x60);
  left_dac.begin(0x61);
}

void get_rear_sensor_angle()
{
  selectI2CChannel(0);
  delay(5);
  uint16_t rear_str_val = readAS5600Angle();
  // Convert raw angle to degrees (AS5600 provides 12-bit resolution)
  //float rear_sensor_angle = (rear_str_val / 4096.0) * 360.0;
  rear_sensor_angle = (rear_str_val / 4096.0) * 360.0;
}

void get_front_sensor_angle()
{
  selectI2CChannel(1);
  delay(5);
  uint16_t front_str_val = readAS5600Angle();
  // Convert raw angle to degrees (AS5600 provides 12-bit resolution)
  //float front_sensor_angle = (front_str_val / 4096.0) * 360.0;
  front_sensor_angle = (front_str_val / 4096.0) * 360.0;
}

void selectI2CChannel(uint8_t channel)
{
  if (channel > 7) return; // PCA9548A supports 8 channels (0-7)  
  Wire.beginTransmission(I2C_MUX_ADDR); 
  Wire.write(1 << channel);  // Select channel 
  Wire.endTransmission();   
}

uint16_t readAS5600Angle() 
{              
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(0x0C);  // Address of the high byte of RAW ANGLE register
  Wire.endTransmission(); 
  Wire.requestFrom(AS5600_ADDR, 2); // Request 2 bytes (MSB + LSB) 
  if (Wire.available() == 2)
  { 
    uint8_t highByte = Wire.read();
    uint8_t lowByte = Wire.read();
    return ((highByte << 8) | lowByte) & 0x0FFF; // 12-bit angle
  }      
  return 0;
}