#include "Steering.h"
#include "Wheel.h"
#include "I2C_Modules.h"
#include "Boom.h"

//#define BOOM_EN 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  #ifdef BOOM_EN
    init_boom_pins();
  #endif

  Serial.begin(115200);
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
  //Serial.print("  | Required Angle=");                                                                                      ////
  //Serial.print(R_str_required_angle); 
  Serial.print("  |FSensor Val= ");                                                                               ////
  Serial.print(front_sensor_angle);                                                                                             ////
  Serial.print("  | RSensor Val= ");                                                                            ////
  Serial.print(rear_sensor_angle);                                                                                             ////  
  Serial.print("  | F Actual Angle=");                                                                                  ////  
  Serial.print(F_str_actual_angle);                                                                                         ////  
  Serial.print("  | R Actual Angle=");                                                                                   ////
  Serial.print(R_str_actual_angle);                                                                                         ////
  //rear_angle_diff= Str_required_angle-R_str_actual_angle;
  Serial.print("  | Rear Diff Angle=");                                                                                   ////
  Serial.print(rear_angle_diff);  
  Serial.print("  | Front Diff Angle=");                                                                                   ////
  Serial.print(front_angle_diff); 
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

