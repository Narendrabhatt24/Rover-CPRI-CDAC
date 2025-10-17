#include <Arduino.h>
#include "I2C_Modules.h"
#define DEBUG

#define power_relay 28                //Wheel +48V Main Supply Control PIN
#define Wheel_direction 29            //Wheel Direction Control PIN



extern volatile bool T4InitBit; 
extern volatile unsigned long T4overflowCount;      //Hold number of overflows
extern volatile unsigned long T4StartTime;          //Hold TIMER value at the starting of Rising edge
extern volatile unsigned long T4FinishTime;         //Hold TIMER value at Starting of falling edge (Or at end of Rised Edge)
extern unsigned long T4ElapsedTime;                 //Hold total number of counts between Start and Finish

extern unsigned int throttle_time;

void init_wheel_pins();
void init_T4_throttle ();
void get_wheel_throttle_time();
void control_power_relay();
void move_wheels_reverse();
void move_wheels_forward();
void stop_wheels();