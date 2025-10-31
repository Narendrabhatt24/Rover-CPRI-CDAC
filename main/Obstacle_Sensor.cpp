#include "Obstacle_Sesnor.h"

unsigned char rx1_data[4];
unsigned char rx2_data[4];
unsigned int interrupt_count1=0;
unsigned int interrupt_count2=0;
bool read_complete1;
bool read_complete2;
unsigned char received_data1;
unsigned char received_data2;

int obst1_val;
int collect_obst1[5];
short int obst1_count=0;
int obst1_calculated=0;
short int error_count1=0;
short int lower_limit_count1=0;

int obst2_val;
int collect_obst2[5];
short int obst2_count=0;
int obst2_calculated=0;
short int error_count2=0;
short int lower_limit_count2=0;

bool obstacle1_status;
bool obstacle2_status;

void Init_Obst1_Sensor(unsigned int ubrr)
{
	UBRR1H = (unsigned char)(ubrr>>8);   // Set baud rate 
	UBRR1L = (unsigned char)ubrr;
	UCSR1B = (1<<RXEN1);                // Enable receiver 
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);   // Set frame format: 8data, 1stop bit 
  SREG = 0b10000000;                  //Global interrupt Enable
  UCSR1B |= (1<<RXCIE1);              //Enable RX Complete Interrupt
  Serial.println("initializing first sensor...");
}

ISR(USART1_RX_vect)
{
  received_data1=UDR1;
  if(received_data1==0XFF && interrupt_count1==0)
  {
    rx1_data[0]=received_data1;
    interrupt_count1++;
  }
  else if(interrupt_count1<=3 && interrupt_count1>0)
  {
    rx1_data[interrupt_count1]=received_data1;
    interrupt_count1++;
    if(interrupt_count1>3)
    {
      interrupt_count1=0;
      read_complete1=1;
    }
  }
}

void Init_Obst2_Sensor(unsigned int ubrr)
{
	UBRR2H = (unsigned char)(ubrr>>8);  	  // Set baud rate 
	UBRR2L = (unsigned char)ubrr;
	UCSR2B = (1<<RXEN2);                    // Enable receiver 
	UCSR2C = (1<<UCSZ20)|(1<<UCSZ21);     	// Set frame format: 8data, 1stop bit 
  UCSR2B |= (1<<RXCIE2);                  //Enable RX Complete Interrupt
  Serial.println("initializing Second Sensor...");
}

ISR(USART2_RX_vect)
{
  received_data2=UDR2;
  if(received_data2==0XFF && interrupt_count2==0)
  {
    rx2_data[0]=received_data2;
    interrupt_count2++;
  }
  else if(interrupt_count2<=3 && interrupt_count2>0)
  {
    rx2_data[interrupt_count2]=received_data2;
    interrupt_count2++;
    if(interrupt_count2>3)
    {
      interrupt_count2=0;
      read_complete2=1;
    }
  }
}

void process_obst_sensor1()
{
  if(read_complete1==1)
  {
    int checksum1;
    checksum1=(rx1_data[0]+rx1_data[1]+rx1_data[2])&0x00FF;
    if(checksum1==rx1_data[3])
    {
      error_count1=0;                                  //Reset error_count value to 0
      lower_limit_count1=0;

      obst1_val=(rx1_data[1]<<8)+rx1_data[2];
      if(obst1_val>30)
      {
        //Serial.print("obst1_val=");
        //Serial.print(obst1_val/10);
        //Serial.print("cm");
        if(obst1_count<5)
        {
          collect_obst1[obst1_count]=obst1_val;
          obst1_count++;
        }
        else if(obst1_count==5)
        {
          obst1_count=0;
          obst1_calculated=((collect_obst1[0]+collect_obst1[1]+collect_obst1[2]+collect_obst1[3]+collect_obst1[4])/5)/10;          
        }
        //obst1_calculated=obst1_val/10;
      }
      else 
      {
        obst1_count=0;
        lower_limit_count1++;                           //Counting Lower limit. Valid only if it remain for 10 times
        if(lower_limit_count1==10)
        {
          lower_limit_count1=0;
          obst1_calculated=0;
          Serial.print("Sensor-1 Below the lower limit");
          delay(10000);
        }  
      }
    }
    else 
    {
      obst1_count=0;
      error_count1++;                           //Count number of errors. Valid only if it remain for 10 times
      if(error_count1==10)
      {
        error_count1=0;
        obst1_calculated=0;
        Serial.print("ERROR in Sensor-1");
        delay(10000);
      }  
    }
  }
  if(obst1_calculated < 100)
    obstacle1_status=1;
  else if (obst1_calculated >= 100)
    obstacle1_status=0;
}

void process_obst_sensor2()
{
  if(read_complete2==1)
  {
    int checksum2;
    checksum2=(rx2_data[0]+rx2_data[1]+rx2_data[2])&0x00FF;
    if(checksum2==rx2_data[3])
    {
      error_count2=0;
      lower_limit_count2=0;
      obst2_val=(rx2_data[1]<<8)+rx2_data[2];
      if(obst2_val>30)
      {
        //Serial.print("obst2_val=");
        //Serial.print(obst2_val/10);
        //Serial.print("cm");
        if(obst2_count<5)
        {
          collect_obst2[obst2_count]=obst2_val;
          obst2_count++;
        }
        else if(obst2_count==5)
        {
          obst2_count=0;
          obst2_calculated=((collect_obst2[0]+collect_obst2[1]+collect_obst2[2]+collect_obst2[3]+collect_obst2[4])/5)/10;          
        }
        //obst2_calculated=obst2_val/10;
      }
      else 
      {
        obst2_count=0;
        lower_limit_count2++;
        if(lower_limit_count2==10)
        {
          lower_limit_count2=0;
          obst2_calculated=0;
          Serial.print("Sensor-2 Below the lower limit");
          delay(10000);
        }
      }
    }
    else 
    {
      obst2_count=0;
      error_count2++;
      if(error_count2==10)
      {
        error_count2=0;
        obst2_calculated=0;
        Serial.print("ERROR in Sensor 2");
        delay(10000);
      }
    }
  }
  if(obst2_calculated < 100)
    obstacle2_status=1;
  else if (obst2_calculated >= 100)
    obstacle2_status=0;
}

