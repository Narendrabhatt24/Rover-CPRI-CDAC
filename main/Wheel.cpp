#include "Wheel.h"

volatile bool T4InitBit; 
volatile unsigned long T4overflowCount;      //Hold number of overflows
volatile unsigned long T4StartTime;          //Hold TIMER value at the starting of Rising edge
volatile unsigned long T4FinishTime;         //Hold TIMER value at Starting of falling edge (Or at end of Rised Edge)
unsigned long T4ElapsedTime;                 //Hold total number of counts between Start and Finish

unsigned int throttle_time;

void init_wheel_pins()
{
  pinMode(Wheel_direction,OUTPUT); 
  pinMode(power_relay,OUTPUT);
  //digitalWrite(Wheel_direction,LOW);
  digitalWrite(Wheel_direction,HIGH);
  //digitalWrite(power_relay,LOW);
  digitalWrite(power_relay,HIGH);
}

void init_T4_throttle ()
{
  T4InitBit = true;
  // Reset Timer 4 
  TCCR4A = 0; 
  TCCR4B = 0; 
  TIFR4 = bit (ICF4) | bit (TOV4);  // clear flags so we don't get a bogus interrupt
  TCNT4 = 0;                        // Counter to zero   
  T4overflowCount = 0;                // Therefore no overflows yet
  // Timer 4 - counts clock pulses  
  TIMSK4 = bit (TOIE4) | bit (ICIE4);   // interrupt on Timer 4 overflow and input capture
  // start Timer 4, no prescaler
  TCCR4B =  bit (CS40) | bit (ICES4);  // plus Input Capture Edge Select (rising on D49)
}

ISR (TIMER4_CAPT_vect)
{
  unsigned int Timer4CounterValue; 
  Timer4CounterValue = ICR4;                        // Store current counter value before it changes any more
  unsigned long T4overflowCopy = T4overflowCount;   // Store overflow count value    
  // if just missed an overflow
  if ((TIFR4 & bit (TOV4)) && Timer4CounterValue < 0x7FFF)
  {  
    T4overflowCopy++;
  } 
  if (T4InitBit)                                   //For Rising edge condition  
  { 
    T4StartTime = (T4overflowCopy << 16) + Timer4CounterValue;
    TIFR4 |= bit (ICF4);     // clear Timer-4, Input Capture Flag
    TCCR4B =  bit (CS40);    // No prescaling, Input Capture Edge Select (falling on D49)
    T4InitBit = false;
    return;
  } 
  T4FinishTime = (T4overflowCopy << 16) + Timer4CounterValue; 
  T4ElapsedTime = T4FinishTime - T4StartTime;
  init_T4_throttle ();
} 

ISR (TIMER4_OVF_vect) 
{ 
  T4overflowCount++;
}

void get_wheel_throttle_time()
{
  throttle_time =(T4ElapsedTime * 62.5e-9 * 1e6);   //Convert count value into us                         ////                                                                                            ////
  if(throttle_time<1100 && throttle_time!=0)                      //Keep in boundary condition                          ////
  {                                                                                                                     ////
    throttle_time=1100;                                                                                                 ////
  }                                                                                                                     ////
  else if(throttle_time>1900)                                     //Keep in boundary condition                          ////
  {                                                                                                                     ////
    throttle_time=1900;                                                                                                 ////
  }
  else if(throttle_time==0)                                       //If remote is off
  {
    throttle_time=1500;
  }
  #ifdef DEBUG
    Serial.print ("   | Throttle=  ");                                                                                    ////
    Serial.print (throttle_time);                                                                                         ////
    Serial.print (" uS. | "); 
  #endif
}

void control_power_relay()
{
  if(throttle_time >1490 && throttle_time<1510)
    //digitalWrite(power_relay,LOW);
    digitalWrite(power_relay,HIGH);
  else
    //digitalWrite(power_relay,HIGH);
    digitalWrite(power_relay,LOW);
}

void move_wheels_reverse()
{
  if(throttle_time>1520 && throttle_time<=1690)          //Forward with No speed change for 1520-1690 throttle value
  {
    int temp5=map(throttle_time,1500,1900,0,3300);
    left_dac.setVoltage(temp5,false);
    right_dac.setVoltage(temp5,false);
    //digitalWrite(Wheel_direction,HIGH);
    digitalWrite(Wheel_direction,LOW);
  }
  else if(throttle_time>1690 && F_str_actual_angle>3)   //Change right side wheel's speed while rover turning right side                                                   
  {                      
    temp1=map(throttle_time,1500,1900,300,3300);     
    int temp2=map(F_str_actual_angle,0,23,3300,2750);   //map angle and DAC relationship | 0 degree ->max speed | 20 -> min speed
    int temp3=map(throttle_time,1500,1900,300,temp2);      

    left_dac.setVoltage(temp1,false);                   //Rotate at normal speed                              
    right_dac.setVoltage(temp3,false);                  //Speed controlled as per turning angle
    //digitalWrite(Wheel_direction,HIGH);               //Wheel moving in forward direction
    digitalWrite(Wheel_direction,LOW); 

    #ifdef DEBUG   
      Serial.print("Forward Right Turning | "); 
      Serial.print("  | Left DAC = ");  
      Serial.print(temp1);  
      Serial.print("  Right DAC ="); 
      Serial.print(temp3);
    #endif 
  }
  else if(throttle_time>1690 && F_str_actual_angle<-3)    //Change left side wheel's speed while rover turning left side
  {
    temp1=map(throttle_time,1500,1900,300,3300);  
    int temp2=map(F_str_actual_angle,0,-31,3300,2750);
    int temp3=map(throttle_time,1500,1900,300,temp2);  
    
    left_dac.setVoltage(temp3,false);                     //Speed controlled as per turning angle
    right_dac.setVoltage(temp1,false);                    //Rotate at normal speed 
    //digitalWrite(Wheel_direction,HIGH);                   //Wheel moving in forward direction
    digitalWrite(Wheel_direction,LOW);
    
    #ifdef DEBUG  
      Serial.print("Forward Left Turning | "); 
      Serial.print("  Left DAC =");
      Serial.print(temp3);                                                             
      Serial.print("  | Right DAC = ");
      Serial.print(temp1);
    #endif

  }
  else if(throttle_time>1690 && (F_str_actual_angle>-3 && F_str_actual_angle<3))  //No speed change for -3 to +3 degree 
  {
    temp1=map(throttle_time,1500,1900,0,3300);
    right_dac.setVoltage(temp1, false); 
    left_dac.setVoltage(temp1, false);
    //digitalWrite(Wheel_direction,HIGH); 
    digitalWrite(Wheel_direction,LOW);

    #ifdef DEBUG  
      Serial.print("Forward Straight | ");
      Serial.print("  Left DAC =");
      Serial.print(temp1); 
      Serial.print("  | Right DAC = ");
      Serial.print(temp1);
    #endif
  } 
}

void move_wheels_forward()
{
  if(throttle_time<1480 && throttle_time>=1310)        //No change in speed for throttle between 1480 to 1310
  {
    int temp4=map(throttle_time,1500,1100,0,3300);
    left_dac.setVoltage(temp4,false); 
    right_dac.setVoltage(temp4,false);
    //digitalWrite(Wheel_direction,LOW);
    digitalWrite(Wheel_direction,HIGH);
  }
  else if(throttle_time<1310 && F_str_actual_angle>3)
  {
    temp1=map(throttle_time,1500,1100,300,3300);                                        ////                              ////
    int temp2=map(F_str_actual_angle,0,23,3300,2750);
    int temp3=map(throttle_time,1500,1100,300,temp2);                                   ////                              ////
    left_dac.setVoltage(temp1,false);                                                 ////                              ////
    right_dac.setVoltage(temp3,false);                                                ////                              ////
    //digitalWrite(Wheel_direction,LOW);                                                ////                              ////
    digitalWrite(Wheel_direction,HIGH);
    
    #ifdef DEBUG  
      Serial.print("Backward Right Turning | ");                                        ////                              ////
      Serial.print("  | Left DAC = ");                                                  ////                              ////
      Serial.print(temp1);                                                              ////                              ////
      Serial.print("  Right DAC =");                                                    ////                              ////
      Serial.print(temp3); 
    #endif
  }
  else if(throttle_time<1310 && F_str_actual_angle<-3)
  {
    temp1=map(throttle_time,1500,1100,300,3300);                                        ////                              ////
    int temp2=map(F_str_actual_angle,0,-31,3300,2750);
    int temp3=map(throttle_time,1500,1100,300,temp2); 
    left_dac.setVoltage(temp3,false);                                                 ////                              ////
    right_dac.setVoltage(temp1,false);                                                ////                              ////
    //digitalWrite(Wheel_direction,LOW);                                                ////                              ////
    digitalWrite(Wheel_direction,HIGH);
    
    #ifdef DEBUG  
      Serial.print("Backward Left Turning | ");                                         ////                              ////  
      Serial.print("  Left DAC =");                                                     ////                              ////  
      Serial.print(temp3);                                                              ////                              ////
      Serial.print("  | Right DAC = ");                                                 ////                              ////
      Serial.print(temp1);
    #endif
  }
  else if(throttle_time<1310 && (F_str_actual_angle<3 && F_str_actual_angle>-3))
  {
    temp1=map(throttle_time,1500,1100,0,3300);                                        ////                              ////
    right_dac.setVoltage(temp1, false);                                               ////                              ////
    left_dac.setVoltage(temp1, false);                                                ////                              ////
    //digitalWrite(Wheel_direction,LOW);                                                ////                              ////
    digitalWrite(Wheel_direction,HIGH);
    
    #ifdef DEBUG  
      Serial.print("Backward Straight | ");                                             ////                              ////
      Serial.print("  Left DAC =");                                                     ////                              ////
      Serial.print(temp1);                                                              ////                              ////
      Serial.print("  | Right DAC = ");                                                 ////                              ////
      Serial.print(temp1);
    #endif
  }
}

void stop_wheels()
{
  right_dac.setVoltage(0, false);
  left_dac.setVoltage(0, false);
  //digitalWrite(Wheel_direction,LOW);
  digitalWrite(Wheel_direction,HIGH);
  
  #ifdef DEBUG  
    Serial.print("Rover Stopped"); 
  #endif
}