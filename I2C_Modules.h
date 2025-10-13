#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Arduino.h>
#include "Steering.h"

extern Adafruit_MCP4725 right_dac;    //DAC For right side motor 
extern Adafruit_MCP4725 left_dac;     //DAC For left side motor

#define I2C_MUX_ADDR  0x70    // Default I2C address of PCA9548A- I2C Mux Module
#define AS5600_ADDR   0x36    // AS5600 I2C address  
#define SENSOR_COUNT  2       // Number of AS5600 sensors connected to PCA9548A- I2C Mux Module 

void init_I2C_modules();
void selectI2CChannel(uint8_t channel);
uint16_t readAS5600Angle();
void get_rear_sensor_angle();
void get_front_sensor_angle();
