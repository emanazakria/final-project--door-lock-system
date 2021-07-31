

#include "main.h"

void Cal_Time(void){
	tick++;
	//according to the used prescalar every 30 tick=1 sec
	if (tick==30){
		Sec_Flag++;
		tick=0;
	}
}

void Receive_Pass(){

	while(UART_recieveByte()!= SAVE){} //Wait till MC1 send SAVE to begin
	uint8 pass; //to receive the entered password from MC1
	uint8 Count_Pass;//Count number of digits(5)
	for(Count_Pass=5; Count_Pass>0; Count_Pass--)// we start from the last digit
	{
		UART_sendByte(M2_Ready); //MC2 is ready to receive the next digit
		pass = UART_recieveByte();
		EEPROM_writeByte((0x0311+Count_Pass), pass); //Save the entered digit in specific address
		_delay_ms(90); //Wait  to write the value in the memory

		/*write the INITIALIZED_FLAG in this address
		 * and we will check this address Every time we run code
		 * to know if the first run code is done or not yet */
		EEPROM_writeByte(0x0420, Init_Flag);
		_delay_ms(90);/*Wait for 90ms to write the value in the memory*/
		UART_sendByte(Saving_Done); //Send to MC1 that saving is done
	}


}


void Check_pass(const Timer_Config * Config_Ptr){

	while(UART_recieveByte()!= Check_Pass); // received from M1 to begin

	uint8 pass=0;       // to receive the entered password from MC1
	uint8 Pass_Saved=0; //to get the right stored password
	uint8 error;         //to know how many times the password was wrong
	uint8 Count_Pass;    //to count the entered password digits(5)
	for(Count_Pass=5; Count_Pass>0; Count_Pass--)
	{
		/*We begin from the last digit entered*/
		pass = UART_recieveByte(); /*Receive the entered password digit from MC1*/

		/*The right password is stored in the EEPROM,
		 *so we go to the address where it should be saved and read the correct value(pass_saved)*/
		EEPROM_readByte((0x0311+Count_Pass), &Pass_Saved);
		if(pass !=Pass_Saved) /*The entered password digit not like that is stored*/
		{
			UART_sendByte(Error_Pass); /*Send to MC1 that there is an error in the password*/
		}
		else /*The entered password digit is like that is stored*/
		{
			/*Send to MC1 to continue sending the remaining digits*/
			UART_sendByte(Continue_Pass);
		}
	}

	error = UART_recieveByte(); //Receive times the password was wrong
	if(error <= 2) // repeat the check function
	{
		Check_pass(Config_Ptr);
	}
	else if(error == 3) //3 times wrong
	{
		//Initialize the timer and pass the required configurations to it
		Timer_init(Config_Ptr);
		tick =0;
		Sec_Flag =0;
		while (Sec_Flag != 60) //light the led , Ring the buzzer
		{
			SET_BIT(PORTD,PD2);
			SET_BIT(PORTD,PD3);
		}
	}

}

void Open_door(const Timer_Config * Config_Ptr){

	while(UART_recieveByte()!= UNLOCK);// received from M1 to begin

	//Initialize the timer and pass the required configurations to it
	Timer_init(Config_Ptr);
	tick =0;
	Sec_Flag =0;
	while(Sec_Flag != 15)//Rotate the motor
	{
		PORTC &= (~(1<<PC2));
		PORTC |= (1<<PC3);
	}
	//count from the beginning
	tick =0;
	Sec_Flag =0;
	//stop the motor
	while(Sec_Flag != 3)
	{
		PORTC &= (~(1<<PC2));
		PORTC &= (~(1<<PC3));
	}
	// count from the beginning
	tick =0;
	Sec_Flag =0;
	// rotate the motor anti-clockwise
	while(Sec_Flag != 15)
	{
		PORTC |= (1<<PC2);
		PORTC &= (~(1<<PC3));
	}

	/*Then stop the motor*/
	CLEAR_BIT(PORTC,PC2);
	CLEAR_BIT(PORTC,PC3);
}
//To handle what user need (change the password or open the gate)
void Main_Options(const Timer_Config * Config_Ptr){

	Check_pass(Config_Ptr);// to check the password
	//password is correct
	if(UART_recieveByte() == 0)
	{
		if(UART_recieveByte() == '*') //change the password
		{
			Receive_Pass();
		}
		else //open the door
		{
			Open_door(Config_Ptr);
		}
	}

	else // the password was entered wrong for 3 times
	{
		CLEAR_BIT(PORTD,PD3);
		CLEAR_BIT(PORTD,PD2);/*buzz , led off as 1 min has passed*/
	}

}


int main(void)
{
	uint8 First_Run_Flag=0;// to check if the code run for the first time or not

	//Initiate as output pins(Input for motor)
	SET_BIT(DDRC,PC2);
	SET_BIT(DDRC,PC3);
	SET_BIT(DDRD,PD2);
	SET_BIT(DDRD,PD3);//LED, BUZZ


	//initial value =zero
	CLEAR_BIT(PORTC,PC2);
	CLEAR_BIT(PORTC,PC3);

	CLEAR_BIT(PORTD,PD2);
	CLEAR_BIT(PORTD,PD3);


	Timer_Config Timer_Config = {timer0,OVERFLOW,CPU_1024,0,0}; //input to the timer structure

	/*Set the call back function pointer in the timer driver
	 to the function that calculate seconds*/
	Timer0_Overflow_CallBack(Cal_Time);

	TWI_Config TWI_Config = {F_CPU_CLOCK,0x02,0x01}; //input to I2C structure

	UART_Config UART_Config = {disabled,bit1,bit8,9600};////input to the UART structure

	EEPROM_init(& TWI_Config);

	UART_init(& UART_Config);

	while(1){

		while(UART_recieveByte()!= M1_Ready); //Wait till MC1 send that it's ready

		//Read the address where the initialized flag should be saved
		EEPROM_readByte(0x0420, & First_Run_Flag);
		UART_sendByte(First_Run_Flag); //Send to MC1 the value of the initialized flag

		if(First_Run_Flag != Init_Flag)//it's not the first run
		{
			Receive_Pass();
		}
		else//first rum
		{

			Main_Options(& Timer_Config);
		}
	}


}



