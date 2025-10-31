#include<Arduino.h>

#define F_CPU 16000000UL
#include <avr/io.h>
#define FOSC 16000000  
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

extern unsigned char rx1_data[4];
extern unsigned char rx2_data[4];
extern bool read_complete1;
extern bool read_complete2;

extern int obst1_val;
extern int collect_obst1[5];
extern short int obst1_count;
extern int obst1_calculated;
extern short int error_count1;
extern short int lower_limit_count1;

extern int obst2_val;
extern int collect_obst2[5];
extern short int obst2_count;
extern int obst2_calculated;
extern short int error_count2;
extern short int lower_limit_count2;

extern bool obstacle1_status;
extern bool obstacle2_status;

void Init_Obst1_Sensor(unsigned int);
void Init_Obst2_Sensor(unsigned int);

void process_obst_sensor1();
void process_obst_sensor2();
