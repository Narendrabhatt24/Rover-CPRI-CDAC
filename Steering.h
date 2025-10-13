#include <Arduino.h>

#define R_str_dir 23          //Rear Steering Direction PIN 
#define R_str_pwm 3          //Rear Steering Speed PIN
#define F_str_dir 22          //Front Steering Direction PIN 
#define F_str_pwm 2          //Front Steering Speed PIN

extern signed int R_str_required_angle;        //Steering required angle (Calculated from Cube+ Orange Roll signal)             
extern signed int F_str_required_angle;        //Steering required angle (Calculated from Cube+ Orange Roll signal)
extern unsigned int rear_sensor_angle;         //Angle calculated by rear sensor
extern unsigned int front_sensor_angle;        //Angle Calculated by front sensor
extern signed int F_str_actual_angle;          //Actual angle of front steering -mapped wrt roll                                              
extern signed int R_str_actual_angle;          //Actual angle of rear steering - mapped wrt roll                                                   
extern unsigned int temp1;                                                                                                   
extern int rear_angle_diff;                    //Rear steering angle difference
extern int front_angle_diff;                   //Front steering angle difference 

extern unsigned int roll_time;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This bit is set to 1 for first time initilization of TIMER-4 (configured to detect Rising Edge)                       ////
//Once rising edge is detected, the TIMER is set to detect Falling edge and this bit is reset to 0.                     ////
extern volatile bool T5InitBit;                                                                                                ////
extern volatile unsigned long T5overflowCount;                                                                                 ////
extern volatile unsigned long T5StartTime;                                                                                     ////  
extern volatile unsigned long T5FinishTime;                                                                                    ////
extern unsigned long T5ElapsedTime;                                                                                            ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_steering_pins();
void init_T5_roll(); 
void get_steering_roll_time();
void control_front_steering();
void control_rear_steering();