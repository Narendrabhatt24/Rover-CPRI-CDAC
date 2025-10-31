#include "Level_Sense.h"

void init_level_sense()
{
  pinMode(Level_1, INPUT_PULLUP);
  pinMode(Level_2, INPUT_PULLUP);
  pinMode(Level_3, INPUT_PULLUP);
  pinMode(Level_4, INPUT_PULLUP);
  pinMode(Level_5, INPUT_PULLUP);
  pinMode(Ch_Lev_PWM,OUTPUT);    
}

void get_level()
{
  // bool L1=digitalRead(Level_1);
  // bool L2=digitalRead(Level_2);
  // bool L3=digitalRead(Level_3);
  // bool L4=digitalRead(Level_4);
  // bool L5=digitalRead(Level_5);

  // if(L1!=0 && L2!=0 && L3!=0 && L4!=0 && L5!=0)
  //   return(0);
  // else if(L1==0 && L2!=0 && L3!=0 && L4!=0 && L5!=0)
  //   return(20);
  // else if(L1==0 && L2==0 && L3!=0 && L4!=0 && L5!=0)
  //   return(40);
  // else if(L1==0 && L2==0 && L3==0 && L4!=0 && L5!=0)
  //   return(60);
  // else if(L1==0 && L2==0 && L3==0 && L4==0 && L5!=0)
  //   return(80);
  // else if(L1==0 && L2==0 && L3==0 && L4==0 && L5==0)
  //   return(100);

  //////////////////////////////////This is a short method/////////////////
  /////////////////////////////////////////////////////////////////////////
  unsigned int value= (digitalRead(Level_1)*20)+(digitalRead(Level_2)*20)+(digitalRead(Level_3)*20)+(digitalRead(Level_4)*20)+(digitalRead(Level_5)*20);
  unsigned int chem_percent= 100-value; 
  #ifdef DEBUG
    Serial.print("  | Chemical level is =" );
    Serial.print(chem_percent);
    Serial.print("%");
  #endif
  send_val_to_cube(chem_percent);  
}

void send_val_to_cube(int a)
{
  digitalWrite(Ch_Lev_PWM,HIGH);
  delayMicroseconds(a*8.50);
  digitalWrite(Ch_Lev_PWM,LOW);
}