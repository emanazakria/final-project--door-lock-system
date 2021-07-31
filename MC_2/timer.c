
#include "timer.h"
#include <avr/interrupt.h>

//global variables to hold the addresses for callback functions

// for timer0
static volatile void (*g_t_0_overflow_callBackPtr)(void) = NULL_PTR;
static volatile void (*g_t_0_compare_callBackPtr)(void) = NULL_PTR;

// for timer1
static volatile void (*g_t_1_overflow_callBackPtr)(void) = NULL_PTR;
static volatile void (*g_t_1_compare_callBackPtr)(void) = NULL_PTR;

// for timer2
static volatile void (*g_t_2_overflow_callBackPtr)(void) = NULL_PTR;
static volatile void (*g_t_2_compare_callBackPtr)(void) = NULL_PTR;

//ISR For Timer 0


ISR(TIMER0_OVF_vect)
{
	if(g_t_0_overflow_callBackPtr != NULL_PTR)
	{
		// Call the Call Back function (using pointer to function )after each overflow
		(*g_t_0_overflow_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if(g_t_0_compare_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function (using pointer to function)
		after the timer reach the compare value*/

		(*g_t_0_compare_callBackPtr)();

	}
}

ISR(TIMER1_OVF_vect)
{
	if(g_t_1_overflow_callBackPtr != NULL_PTR)
	{
		// Call the Call Back function (using pointer to function) after each overflow
		(*g_t_1_overflow_callBackPtr)();
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(g_t_1_compare_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function (using pointer to function)
		after the timer reach the compare value*/
		(*g_t_1_compare_callBackPtr)();
	}
}

ISR(TIMER2_OVF_vect)
{
	if(g_t_2_overflow_callBackPtr != NULL_PTR)
	{
		// Call the Call Back function (using pointer to function) after each overflow
		(*g_t_2_overflow_callBackPtr)();
	}
}
ISR(TIMER2_COMP_vect)
{
	if(g_t_2_compare_callBackPtr != NULL_PTR)
	{

		/* Call the Call Back function (using pointer to function)
		after the timer reach the compare value*/

		(*g_t_2_compare_callBackPtr)();
	}
}



//Function that initialize the timer

void Timer_init(const Timer_Config * Config_Ptr)
{
	CLEAR_BIT(SREG,7); //Disable I-bit at the beginning

	uint16 timer_number,mode,scalar;
	mode = Config_Ptr->mode;
	timer_number = Config_Ptr->num;


	/*prescalars in the enum is for timer2
	 * so we'll customize them for timer0 and timer1
	 * for timer0 and timer1 we'll use scalar
	 * but for timer2 we'll use Config_Ptr->scalar */
	scalar = Config_Ptr->scalar;
	switch(scalar)
	{
	case 4: //if it's 4 (clk/64) then it's 3 in Timers 0 & 1
		scalar =3;
		break;
	case 6: //if it's 6 (clk/256) then it's 4 in Timers 0 & 1
		scalar =4;
		break;
	case 7: //if it's 7 (clk/1024) then it's 5 in Timers 0 & 1
		scalar =5;
		break;
	}

	if(timer_number == 0) //timer0
	{
		//we assume it's a normal mode at the beginning
		SET_BIT(TCCR0,FOC0);//Non_PWM mode
		if(mode == 1)//compare
		{
			TCCR0 |= (1<<WGM01); // compare mode
			OCR0 = Config_Ptr->s_comp; //get the compare value
		}
		TCCR0 = (TCCR0 & 0xF8) |scalar;// put the prescalar value in the first 3bits
		TCNT0 = Config_Ptr->s_initial;// the initial value
		if(mode == 1) // compare
		{
			SET_BIT(TIMSK,OCIE0);//enable the output compare match interrupt
		}
		else// overflow
		{
			SET_BIT(TIMSK,TOIE0);//enable the overflow interrupt
		}
	}

	else if(timer_number == 1) // timer1
	{
		// Non_PWM mode
		SET_BIT(TCCR1A,FOC1A);
		SET_BIT(TCCR1A,FOC1B);
		if(mode == 1)//compare
		{
			TCCR1B = (1<<WGM12); // Compare mode
			OCR1A = Config_Ptr->s_comp; //get the compare value
		}
		TCCR1B = (TCCR1B & 0xF8) | scalar; // put the prescalar value in the first 3bits
		TCNT1 = Config_Ptr->s_initial; // the initial value
		if(mode==1)//compare
		{
			SET_BIT(TIMSK,OCIE1A);//enable the output compare A match interrupt
		}

		else //overflow
		{
			SET_BIT(TIMSK,TOIE1);//enable the overflow interrupt
		}

	}
	else //timer2
	{
		SET_BIT(TCCR2,FOC2);//Non_PWM mode
		if(mode==1)//compare
		{
			TCCR2 |= (1<<WGM21); //compare mode
			OCR2 = Config_Ptr->s_comp; //get the compare value
		}
		TCCR2 = (TCCR2 & 0xF8) |scalar;// put the prescalar value in the first 3bits
		TCNT2 = Config_Ptr->s_initial;// the initial value
		if(mode==1)//compare
		{
			SET_BIT(TIMSK,OCIE2);//enable the output compare match interrupt
		}
		else //overflow
		{
			SET_BIT(TIMSK,TOIE2);//enable the overflow interrupt
		}


	}

	SET_BIT(SREG,7); //enable I-bit
}//end of init function


//Functions to set the Call Back function address

//for Timer0 at overflow mode
void Timer0_Overflow_CallBack(void(*a_t0_overflow_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_0_overflow_callBackPtr = a_t0_overflow_ptr;
}


//for Timer0 at compare mode
void Timer0_Compare_CallBack(void(*a_t0_compare_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_0_compare_callBackPtr = a_t0_compare_ptr;
}

//for Timer1 at overflow mode

void Timer1_Overflow_CallBack(void(*a_t1_overflow_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_1_overflow_callBackPtr = a_t1_overflow_ptr;
}

//for Timer1 at compare mode
void Timer1_Compare_CallBack(void(*a_t1_compare_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_1_compare_callBackPtr = a_t1_compare_ptr;
}


//for Timer2 at overflow mode

void Timer2_Overflow_CallBack(void(*a_t2_overflow_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_2_overflow_callBackPtr = a_t2_overflow_ptr;
}


//for Timer0 at compare mode
void Timer2_Compare_CallBack(void(*a_t2_compare_ptr)(void))
{
	// Save the address of the Call back function in a global variable
	g_t_2_compare_callBackPtr = a_t2_compare_ptr;
}
