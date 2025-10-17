#include <Arduino.h>

#define Boom_UD_pwm 4
#define Boom_LR_pwm 6
#define Boom_UD_dir 24
#define Boom_LR_dir 26
#define Boom_LR_ip  45
#define Boom_UD_ip  46


void init_boom_pins(void);
void get_boom_UD_time(void);
void get_boom_LR_time(void);
void control_boom_UD(void);
void control_boom_LR(void);

extern unsigned int boom_UD_time; 
extern unsigned int boom_LR_time;

