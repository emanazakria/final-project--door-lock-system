#include "lcd.h"
#include "keypad.h"
#include "UART.h"
#include "timer.h"

//definations
#define M2_Ready 0x10 //MC1 sends to MC2 when MC2 it's ready
#define M1_Ready 0x20 //MC2 sends to MC1 when MC1 it's ready
#define Init_Flag   0x01 //flag saved first time code is run
#define Error_Pass 0x00  // if the entered password is wrong
#define SAVE 0x03 //MC1 sends to MC2 to start saving the password
#define Saving_Done 0X05 //MC2 sends to MC1 when the password is saved
#define Check_Pass 0x07 //MC1 send to MC2 to check the password
#define UNLOCK 0x09 //MC1 send to MC2 to open the gate

//global variables
uint8 Sec_Flag=0; //indicates number of seconds
uint8 Error_Counter=0; // counts number of errors
uint16 tick =0; //number of overflows
uint8 return_flag=0; //to know if the password has been entered for 3 times wrong or not

//functions

void Cal_Time(void);

void First_Run();

void Check_pass(const Timer_Config * Config_Ptr);

void Open_gate(const Timer_Config *Config_Ptr);

void Main_Options(const Timer_Config *Config_Ptr);







