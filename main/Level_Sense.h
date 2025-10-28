#include <Arduino.h>

#define DEBUG

#define Level_1 30
#define Level_2 31
#define Level_3 32
#define Level_4 33
#define Level_5 34

#define Ch_Lev_PWM 8

void init_level_sense();
void get_level();
void send_val_to_cube(int a);
