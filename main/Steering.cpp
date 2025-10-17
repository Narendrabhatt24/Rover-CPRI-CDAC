
#include "Steering.h"

signed int R_str_required_angle;        //Steering required angle (Calculated from Cube+ Orange Roll signal)             
signed int F_str_required_angle;        //Steering required angle (Calculated from Cube+ Orange Roll signal)
unsigned int rear_sensor_angle;         //Angle calculated by rear sensor
unsigned int front_sensor_angle;        //Angle Calculated by front sensor
signed int F_str_actual_angle;          //Actual angle of front steering -mapped wrt roll                                              
signed int R_str_actual_angle;          //Actual angle of rear steering - mapped wrt roll                                                   
unsigned int temp1=0;                                                                                                   
int rear_angle_diff;                    //Rear steering angle difference
int front_angle_diff;                   //Front steering angle difference 

int temp_roll_f,temp_roll_r;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This bit is set to 1 for first time initilization of TIMER-4 (configured to detect Rising Edge)                       ////
//Once rising edge is detected, the TIMER is set to detect Falling edge and this bit is reset to 0.                     ////
volatile bool T5InitBit;                                                                                                ////
volatile unsigned long T5overflowCount;                                                                                 ////
volatile unsigned long T5StartTime;                                                                                     ////  
volatile unsigned long T5FinishTime;                                                                                    ////
unsigned long T5ElapsedTime;                                                                                            ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int roll_time;
void init_steering_pins()
{
  pinMode(F_str_dir,OUTPUT);
  pinMode(F_str_pwm,OUTPUT);
  pinMode(R_str_dir,OUTPUT);
  pinMode(R_str_pwm,OUTPUT);

  digitalWrite(F_str_dir,LOW);
  analogWrite(F_str_pwm,0);
  digitalWrite(R_str_dir,LOW);
  analogWrite(R_str_pwm,0);
}

void init_T5_roll() 
{            
  T5InitBit=true;
  TCCR5A=0; 
  TCCR5B=0;
  TIFR5 = bit (ICF5) | bit (TOV5);  // clear flags so we don't get a bogus interrupt  
  TCNT5 = 0;                        // Counter to zero
  T5overflowCount = 0;               // Therefore no overflows yet 
  // Timer 5 - counts clock pulses   
  TIMSK5 = bit (TOIE5) | bit (ICIE5);   // interrupt on Timer 5 overflow and input capture 
  // start Timer 5, no prescaler 
  TCCR5B =  bit (CS50) | bit (ICES5);  // Plus Input Capture Edge Select (rising on D48)
} 

ISR (TIMER5_OVF_vect)                                                         
{                                                                             
  T5overflowCount++;                                                          
}  

ISR (TIMER5_CAPT_vect)                                                                                                    
{                                                                                                                       
  unsigned int timer5CounterValue;                                                                                        
  timer5CounterValue = ICR5;                        // Store current counter value before it changes any more            
  unsigned long T5overflowCopy = T5overflowCount;   // Store overflow count value                  
  // if just missed an overflow                                                                     
  if ((TIFR5 & bit (TOV5)) && timer5CounterValue < 0x7FFF)
  {
    T5overflowCopy++;                                                                                                     
  }
  if (T5InitBit)                                    //For Rising edge condition 
  {                                                           
    T5StartTime = (T5overflowCopy << 16) + timer5CounterValue; 
    TIFR5 |= bit (ICF5);     // clear Timer-5 Input Capture Flag    
    TCCR5B =  bit (CS50);    // No prescaling, Input Capture Edge Select (falling on D48)
    T5InitBit = false;
    return; 
  } 
  T5FinishTime = (T5overflowCopy << 16) + timer5CounterValue;
  T5ElapsedTime = T5FinishTime - T5StartTime;
  init_T5_roll ();
}

void get_steering_roll_time()
{
  roll_time =(T5ElapsedTime * 62.5e-9 * 1e6);       //Convert count value into us                         ////                                                                                             ////
  if(roll_time<1100 && roll_time!=0)                            //Keep in boundary condition                            ////
  {                                                                                                                     ////
    roll_time=1100;                                                                                                     ////
  }                                                                                                                     ////
  else if(roll_time>1900)                                       //Keep in boundary condition                            //// 
  {                                                                                                                     ////
    roll_time=1900;                                                                                                     ////
  }
  else if(roll_time==0)                                         //if remote is off
  {
    roll_time=1500;
  }
  #ifdef DEBUG
    Serial.println (""); 
    Serial.print ("Roll :");
    Serial.print (roll_time);
    Serial.print (" uS. ");
  #endif
}

void control_front_steering()
{
  ////////////////////////////////////////////////////////  ||   1100----- 1500 -----1900   || Remote Range
  ////////////////////////////////////////////////////////  ||   228 ----- 195  -----172     || Front Wheel Angle Sensor Value Range
  ////////////////////////////////////////////////////////  ||   33  -----  0   -----23     || Actual Angle of front wheels
  ////////////////////////////////////////////////////////  ||    \\        ||       //     || Front Wheels orientation
  ////////////////////////////////////////////////////////  ||     \\       ||      //      ||
  
  ///?????????TO DO ---> Combilne motion in single if else & roll time compare condition in another if else
  
  if(roll_time>=1500)                                                       //Condition for roll time above 1500
  {
    temp_roll_f=roll_time;                                                  //23 degree mapping out of 60 degree
    if(temp_roll_f>1654)
    {
      temp_roll_f=1654;
    }
    F_str_required_angle=map(temp_roll_f,1500,1654,0,23);                     // Steering mapping wrt remote 
    F_str_actual_angle=map(front_sensor_angle,195,172,0,23);                 // Steering mapping wrt sensor
    front_angle_diff= F_str_required_angle-F_str_actual_angle;              // Difference in actual and required angle
 
    if(front_angle_diff>4)
    {
      digitalWrite(F_str_dir,LOW);                                          //Clock/Right direction
      analogWrite(F_str_pwm,255);                                           //Full speed
      //Serial.print(" || 1st running ||");
    }
    else if(front_angle_diff<=4 && front_angle_diff>0)
    {
      digitalWrite(F_str_dir,LOW);                                          //Clock/Right direction
      analogWrite(F_str_pwm,60);                                            //Low Speed
      //Serial.print(" || 2nd running ||");
    }
    else if (front_angle_diff>=-4 && front_angle_diff<0)
    {
      digitalWrite(F_str_dir,HIGH);                                         //Anti Clock/Left Direction
      analogWrite(F_str_pwm,60);                                            //Low Speed
      //Serial.print(" || 3rd running ||");
    }
    else if(front_angle_diff<-4)
    {
      digitalWrite(F_str_dir,HIGH);                                         //Anti Clock/Left Direction
      analogWrite(F_str_pwm,255);                                           //Full Speed
      //Serial.print(" || 4th running ||");   
    }
    else                                                                    //Stop if difference is 0                                                                   
    {
      digitalWrite(F_str_dir,LOW);                                          
      digitalWrite(F_str_pwm,LOW);
      //Serial.print(" || 5th running ||"); 
    }
  }

  else if(roll_time<1500 )                                                   //Condition for roll time below 1500
  {
    temp_roll_f=roll_time;
    if(temp_roll_f<1293)                                                    //-31 degree mapping out of 60 degree
    {
      temp_roll_f=1293;                                                     //Boundary condition
    }
    F_str_required_angle=map(temp_roll_f,1500,1293,0,-31);                    //Steering mapping wrt remote
    F_str_actual_angle=map(front_sensor_angle,195,226,0,-31);               //Steering mapping wrt sensor
    front_angle_diff= F_str_required_angle-F_str_actual_angle;              //Difference in required and actual angle

    if(front_angle_diff>4)
    {
      digitalWrite(F_str_dir,LOW);                                          //Clock/Right Direction
      analogWrite(F_str_pwm,255);                                           //Full speed
      //Serial.print(" || 1st running ||");
    }
    else if(front_angle_diff<=4 && front_angle_diff>0)
    {
      digitalWrite(F_str_dir,LOW);                                          //Clock/Right Direction
      analogWrite(F_str_pwm,60);                                            //Slow Speed
      //Serial.print(" || 2nd running ||");
    }
    else if (front_angle_diff>=-4 && front_angle_diff<0)
    {
      digitalWrite(F_str_dir,HIGH);                                         //Anti-Clock/Left Direction
      analogWrite(F_str_pwm,60);                                            //Slow Speed
      //Serial.print(" || 3rd running ||");
    }
    else if(front_angle_diff<-4)
    {
      digitalWrite(F_str_dir,HIGH);                                         //Anti-Clock/Left Direction
      analogWrite(F_str_pwm,255);                                           //Fast speed
      //Serial.print(" || 4th running ||");    
    }
    else                                                                    ////Stop if difference is 0
    {
      digitalWrite(F_str_dir,LOW);
      digitalWrite(F_str_pwm,LOW);
      //Serial.print(" || 5th running ||");
      //analogWrite(R_str_pwm,0);   
    }
  }
  #ifdef DEBUG
    Serial.print("  |FSensor Val= ");                                                                               ////
    Serial.print(front_sensor_angle);
    Serial.print("  | F Actual Angle=");                                                                                  ////  
    Serial.print(F_str_actual_angle);
    Serial.print("  | Front Diff Angle=");                                                                                   ////
    Serial.print(front_angle_diff); 
  #endif
}

void control_rear_steering()
{
  ////////////////////////////////////////////////////////  ||   1100----- 1500 -----1900   || Remote Range
  ////////////////////////////////////////////////////////  ||   43  -----  73  -----94     || Rear Wheel Angle Sensor Value Range
  ////////////////////////////////////////////////////////  ||   30  -----  0   -----21     || Actual Angle of Rear wheels
  ////////////////////////////////////////////////////////  ||   //         ||       \\     || Rear Wheels orientation wrt angles
  ////////////////////////////////////////////////////////  ||  //          ||        \\    ||
                                                              // 235        265       285
  ///?????????TO DO ---> Combilne motion in single if else & roll time compare condition in another if else

  if(roll_time>=1500)                                           //Condition for roll time above 1500
  {
    temp_roll_r= roll_time;                                     ///19 degree mapping out of 60 degree
    if(temp_roll_r>1626)
    {
      temp_roll_r=1626;
    }
    R_str_required_angle=map(temp_roll_r,1500,1626,0,19);         //Steering mapping wrt remote
    R_str_actual_angle=map(rear_sensor_angle,265,284,0,19);       //Steering mapping wrt snesor
    rear_angle_diff= R_str_required_angle-R_str_actual_angle;   //Difference in required and actual angle

    if(rear_angle_diff>4)
    {
      digitalWrite(R_str_dir,LOW);                              //Anti-Clock/Left Direction
      analogWrite(R_str_pwm,255);                               //Full Speed
      //Serial.print(" || 1st running ||");
    }
    else if(rear_angle_diff<=4 && rear_angle_diff>0)
    {
      digitalWrite(R_str_dir,LOW);                              //Anti-Clock/Left Direction
      analogWrite(R_str_pwm,60);                                //Slow Speed
      //Serial.print(" || 2nd running ||");
    }
    else if (rear_angle_diff>=-4 && rear_angle_diff<0)
    {
      digitalWrite(R_str_dir,HIGH);                             //Clock/Right Direction
      analogWrite(R_str_pwm,60);                                //Slow Speed
      //Serial.print(" || 3rd running ||");
    }
    else if(rear_angle_diff<-4)
    {
      digitalWrite(R_str_dir,HIGH);                             //Clock/Right Direction
      analogWrite(R_str_pwm,255);                               //Full Speed
      //Serial.print(" || 4th running ||");   
    }
    else                                                        //Stop if difference is 0
    {
      digitalWrite(R_str_dir,LOW);
      digitalWrite(R_str_pwm,LOW);
      //Serial.print(" || 5th running ||");
      //analogWrite(R_str_pwm,0);   
    }
  }

  else if(roll_time<1500)                                       //Condition for roll time above 1500
  {
    temp_roll_r= roll_time;                                     //-30 degree mapping out of 60 degree
    if(temp_roll_r<1300)
    {
      temp_roll_r=1300;
    }
    R_str_required_angle=map(temp_roll_r,1500,1300,0,-30);        //Steering Mapping wrt remote
    R_str_actual_angle=map(rear_sensor_angle,265,235,0,-30);      //Steering Mapping wrt sensor
    rear_angle_diff= R_str_required_angle-R_str_actual_angle;     //Difference in required and actual angle

    if(rear_angle_diff>4)
    {
      digitalWrite(R_str_dir,LOW);                              //Anti-Clock/Left Direction
      analogWrite(R_str_pwm,255);                               //Full Speed
      //Serial.print(" || 1st running ||");
    }
    else if(rear_angle_diff<=4 && rear_angle_diff>0)
    {
      digitalWrite(R_str_dir,LOW);                              //Anti-Clock/Left Direction
      analogWrite(R_str_pwm,60);                                //Slow Speed
      //Serial.print(" || 2nd running ||");                   
    }
    else if (rear_angle_diff>=-4 && rear_angle_diff<0)
    {
      digitalWrite(R_str_dir,HIGH);                             //Clock/Right Direction
      analogWrite(R_str_pwm,60);                                //Slow Speed
      //Serial.print(" || 3rd running ||");
    }
    else if(rear_angle_diff<-4)
    {
      digitalWrite(R_str_dir,HIGH);                             //Clock/Right Direction
      analogWrite(R_str_pwm,255);                               //Fast Speed
      //Serial.print(" || 4th running ||");    
    }
    else                                                        //Stop if difference is 0
    {
      digitalWrite(R_str_dir,LOW);
      digitalWrite(R_str_pwm,LOW);
      //Serial.print(" || 5th running ||");
      //analogWrite(R_str_pwm,0);   
    }
  }
  #ifdef DEBUG
    Serial.print("  | RSensor Val= ");                                                                            ////
    Serial.print(rear_sensor_angle);                                                                                             ////                                                                                        ////  
    Serial.print("  | R Actual Angle=");                                                                                   ////
    Serial.print(R_str_actual_angle);                                                                                         ////
    Serial.print("  | Rear Diff Angle=");                                                                                   ////
    Serial.print(rear_angle_diff);  
  #endif
}