#include "Steering.h"
#include "Wheel.h"
#include "I2C_Modules.h"
#include "Boom.h"
#include "Level_Sense.h"
#include "Obstacle_Sesnor.h"


#define OBSTACLE_EN 
//#define BOOM_EN 
#define LEVEL_SENSE_EN
#define DEBUG                     //Also defined in Wheel.h and Steering.h and Level_Sense.h for individual control
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  #ifdef OBSTACLE_EN
    Init_Obst1_Sensor(MYUBRR);
    Init_Obst2_Sensor(MYUBRR);
  #endif  
  
  #ifdef LEVEL_SENSE_EN
    init_level_sense();
  #endif

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
  #ifdef OBSTACLE_EN
    process_obst_sensor1();
    Serial.print("    |");
    Serial.print("  Distance from obst1 sensor =");
    Serial.print(obst1_calculated);
    Serial.print("  |");
    /////////////////////Process Sensor-2//////////////////////
    process_obst_sensor2(); 
    Serial.print("    |");
    Serial.print("  Distance from obst2 sensor =");
    Serial.print(obst2_calculated);
    Serial.print("  |");
  #else
    obstacle1_status=0;
    obstacle2_status=0;
  #endif  

  get_steering_roll_time();
  get_rear_sensor_angle();
  get_front_sensor_angle();
  control_front_steering();
  control_rear_steering();

  get_wheel_throttle_time();
  control_power_relay();
  if(throttle_time>1520 && throttle_time<=1900 && obstacle1_status==0 && obstacle2_status==0)
    move_wheels_reverse();
  else if(throttle_time<1480 && throttle_time>=1100 && obstacle1_status==0 && obstacle2_status==0)
    move_wheels_forward();
  else
    stop_wheels();

  #ifdef LEVEL_SENSE_EN
    get_level();
  #endif
  
  #ifdef BOOM_EN
    get_boom_UD_time();
    control_boom_UD();
    get_boom_LR_time();
    control_boom_LR();  
  #endif

}

