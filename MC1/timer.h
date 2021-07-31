
#ifndef TIMERS_H_
#define TIMERS_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


//types' declaration to timer config.

typedef enum
{
	timer0, timer1 , timer2
}Timer_Number;

typedef enum
{ OVERFLOW, COMPARE
}Timer_Mode;

typedef enum
{
	NO_CLOCK,CPU_CLOCK,CPU_8,CPU_32,CPU_64,CPU_128,CPU_256,CPU_1024
}Timer_Prescalar;

typedef struct {


	Timer_Number num;
	Timer_Mode mode;
	Timer_Prescalar scalar;
	uint16 s_initial; //Take the initial value from the user
	uint16 s_comp;
}Timer_Config;


//functions

// Function that initialize the timer
void Timer_init(const Timer_Config * Config_Ptr);


//for the Call Back function address for Timer0 at the overflow mode
void Timer0_Overflow_CallBack(void(*a_t0_overflow_ptr)(void));

//for the Call Back function address for Timer0 at the compare mode
void Timer0_Compare_CallBack(void(*a_t0_compare_ptr)(void));

//for the Call Back function address for Timer1 at the overflow mode
void Timer1_Overflow_CallBack(void(*a_t1_overflow_ptr)(void));

//for the Call Back function address for Timer1 at the compare mode
void Timer1_Compare_CallBack(void(*a_t1_compare_ptr)(void));

//for the Call Back function address for Timer2 at the overflow mode
void Timer2_Overflow_CallBack(void(*a_t2_overflow_ptr)(void));

//for the Call Back function address for Timer2 at the compare mode
void Timer2_Compare_CallBack(void(*a_t2_compare_ptr)(void));



#endif /* TIMERS_H_ */

