#include "Boom.h"

unsigned int boom_UD_time=0;
unsigned int boom_LR_time=0;

void init_boom_pins()
{
  pinMode(Boom_LR_ip,INPUT);
  pinMode(Boom_UD_ip,INPUT);
  pinMode(Boom_UD_pwm,OUTPUT);
  pinMode(Boom_LR_pwm,OUTPUT);
  pinMode(Boom_UD_dir,OUTPUT);
  pinMode(Boom_LR_dir,OUTPUT);

  digitalWrite(Boom_UD_pwm,LOW);
  digitalWrite(Boom_UD_dir,LOW);
  digitalWrite(Boom_LR_pwm,LOW); 
  digitalWrite(Boom_LR_dir,LOW);  
}

void get_boom_UD_time()
{
  boom_UD_time=pulseIn(Boom_UD_ip,HIGH);
  if(boom_UD_time<1100)
  {
    boom_UD_time=1100;
  }
  else if(boom_UD_time>1900)
  {
    boom_UD_time=1900;
  }
  Serial.print("Boom Up down Time =");
  Serial.print(boom_UD_time);
}

void get_boom_LR_time()
{
  boom_LR_time=pulseIn(Boom_LR_ip,HIGH);
  if(boom_LR_time<1100)
  {
    boom_LR_time=1100;
  }
  else if(boom_LR_time>1900)
  {
    boom_LR_time=1900;
  }
  Serial.print("Boom Left Right Time =");
  Serial.print(boom_LR_time);
}

void control_boom_UD()
{
  if(boom_UD_time<1400)
  {
    digitalWrite(Boom_UD_pwm,HIGH);
    digitalWrite(Boom_UD_dir,LOW);
  }
  else if(boom_UD_time>1600)
  {
    digitalWrite(Boom_UD_pwm,HIGH);
    digitalWrite(Boom_UD_dir,HIGH);
  }
  else
  {
    digitalWrite(Boom_UD_pwm,LOW);
    digitalWrite(Boom_UD_dir,LOW);
  }
}

void control_boom_LR()
{
  if(boom_LR_time<1400)
  {
    digitalWrite(Boom_LR_pwm,HIGH);
    digitalWrite(Boom_LR_dir,LOW);
  }
  else if(boom_LR_time>1600)
  {
    digitalWrite(Boom_LR_pwm,HIGH);
    digitalWrite(Boom_LR_dir,HIGH);
  }
  else
  {
    digitalWrite(Boom_LR_pwm,LOW);
    digitalWrite(Boom_LR_dir,LOW);    
  }  
}