#include "Steering.h"
#include "Wheel.h"
#include "I2C_Modules.h"
#include "Boom.h"

//#define BOOM_EN 
//#define DEBUG                     //Also defined in Wheel.h and Steering.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  #ifdef BOOM_EN
    init_boom_pins();
  #endif

  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  init_steering_pins();
  init_wheel_pins();
  init_T5_roll ();
  init_T4_throttle ();
  init_I2C_modules();
  delay(7000); 
}

void loop() 
{
  get_steering_roll_time();
  get_rear_sensor_angle();
  get_front_sensor_angle();
  control_front_steering();
  control_rear_steering();

  get_wheel_throttle_time();
  control_power_relay();
  if(throttle_time>1520 && throttle_time<=1900)
    move_wheels_reverse();
  else if(throttle_time<1480 && throttle_time>=1100)
    move_wheels_forward();
  else
    stop_wheels();

  #ifdef BOOM_EN
    get_boom_UD_time();
    control_boom_UD();
    get_boom_LR_time();
    control_boom_LR();  
  #endif
}

