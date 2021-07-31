#include "main.h"
void Cal_Time(void){
	tick++;
	//according to the used prescalar every 30 tick=1 sec
	if (tick==30){
		Sec_Flag++;
		tick=0;
	}
}

// in this function the user enters the password for 2 times,then MC1 sends it to MC2 to save it
void First_Run()
{
	sint32 New_Pass = 0; //to save the entered new password
	sint32 Re_Pass = 0;  // to save the second try password
	uint16 key = 0;       //to save the pressed key from the keypad
	uint16 pass=0;       //to save the password digit and send it to M2
	uint16 Count_Pass;  //to count entered password digits (5)

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter new pass:");

	for(Count_Pass=0; Count_Pass<5; Count_Pass++) //To take the 5 digits
	{
		key = KeyPad_getPressedKey();
		while(!((key >= 0) && (key <= 9)))//check if it's a number from 0 to 9
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,Count_Pass,"*"); //Display Digit as *
		New_Pass= New_Pass*10 + key; //Save the 5 digits
		_delay_ms(2200); //press time
	}

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Re-enter pass:");

	for(Count_Pass=0; Count_Pass<5; Count_Pass++)//take the 5 digits again
	{
		key = KeyPad_getPressedKey();

		while(!((key >= 0) && (key <= 9)))
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,Count_Pass,"*");/*Display the pressed key as (*)*/
		Re_Pass= Re_Pass*10 + key; //Save the 5 digits /
		_delay_ms(2200); //press time
	}
	//compare the 2 passwords
	if (New_Pass==Re_Pass)
	{
		UART_sendByte(SAVE); //to start saving

		for(Count_Pass=0;Count_Pass<5;Count_Pass++)
		{
			pass = Re_Pass % 10; // the last digit from right
			while(UART_recieveByte() != M2_Ready){} //Wait till MC2 become ready
			UART_sendByte(pass); //MC1 sends to MC2 the digit to save it
			Re_Pass /= 10; //To get the next digit to right
		}
		while(UART_recieveByte () != Saving_Done){} //Wait until MC2 send that it finished saving

	}
	else //passwords don't match
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Incorrect pass");
		_delay_ms(1500);
		First_Run(); //Repeat the function
	}
}

void Check_pass(const Timer_Config * Config_Ptr)
{
	sint32 pass=0; //to save the entered 5 digits password
	uint16 Count_Pass;  //to count entered password digits (5)
	uint16 key = 0;       //to save the pressed key from the keypad
	uint16 Pass_Digit=0; //save the password digit and pass it to MC2 to check it
	uint16 error=0; //to check if there is an error in any digit

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter pass:");

	for(Count_Pass=0; Count_Pass<5; Count_Pass++)
	{
		key = KeyPad_getPressedKey();
		while(!((key >= 0) && (key <= 9)))
		{
			key = KeyPad_getPressedKey();
		}
		LCD_displayStringRowColumn(1,Count_Pass,"*"); //Display the pressed key as *
		pass= pass*10 + key; //Save the 5 digits
		_delay_ms(2200);
	}

	UART_sendByte(Check_Pass); //MC1 send to MC2 to check the password

	for(Count_Pass=0; Count_Pass<5; Count_Pass++)
	{
		Pass_Digit = pass % 10; // the last digit from right
		UART_sendByte(Pass_Digit);
		if( UART_recieveByte() == Error_Pass)// the sent digit is wrong
		{
			error ++; //count number of errors untill 3
		}
		pass /= 10; // to get the next digit from right
	}

	/*if error = zero,this means that the password is correct
	 * If it 1 or more , we will increase Error_Counter and repeat the function
	 * If counter_error became 3, the LCD will display error message for 1 minute*/
	if (error != 0) //The password is wrong
	{
		Error_Counter ++;
		UART_sendByte(Error_Counter);
		/*MC1 send the value of the error counter to MC2
		 *If the error counter is less than 3
		 *it will display a message and repeat the check function*/
		if(Error_Counter < 3)
		{
			LCD_clearScreen(); /*Clear LCD*/
			LCD_displayStringRowColumn(0,0,"Wrong pass");
			_delay_ms(1500);
			Check_pass(Config_Ptr); //Repeat the check function
		}
		else //user entered password wrong for 3 times)
		{
			LCD_clearScreen();

			//Initialize the timer and pass the required configurations to it
			Timer_init(Config_Ptr);
			tick =0;
			Sec_Flag =0;
			while (Sec_Flag != 60) // error message for 1 minute
			{
				LCD_displayStringRowColumn(0,0,"Wrong pass");
			}

			Error_Counter = 0;
			return_flag =1; //the user has entered the password wrong for 3 times*/
		}
	}
	else //The password is right
	{ /*MC1 should send number of errors
			   so 4 means that no error in the entered password*/
		UART_sendByte(4);
		Error_Counter = 0; // to count correct the next run
	}
}
void Open_gate(const Timer_Config *Config_Ptr)
{

	UART_sendByte(UNLOCK); //MC1 sends to MC2 to open the gate

	LCD_clearScreen();
	//Initialize the timer and pass the required configurations to it
	Timer_init(Config_Ptr);
	tick =0;
	Sec_Flag =0;
	while(Sec_Flag != 15)//openning the door
	{
		LCD_displayStringRowColumn(0,0,"opening the door");
	}
	LCD_clearScreen();

	tick =0;
	Sec_Flag =0;
	while(Sec_Flag != 3)//door opened
	{
		LCD_displayStringRowColumn(0,0,"Door is opened");
	}

	LCD_clearScreen();
	tick =0;
	Sec_Flag =0;
	while(Sec_Flag != 15)//closing the door
	{
		LCD_displayStringRowColumn(0,0,"closing door");
	}



}
void Main_Options(const Timer_Config *Config_Ptr){

	uint16 key = 0;       //to save the pressed key from the keypad

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"*:Change password");
	LCD_displayStringRowColumn(1,0,"%:Open the door");

	key = KeyPad_getPressedKey();

	while((key != '*') && (key != '%'))//check if it's right digit
	{
		key = KeyPad_getPressedKey();
	}

	Check_pass(Config_Ptr);
	UART_sendByte(return_flag);


	if(return_flag == 0)
	{
		UART_sendByte(key);
		if (key == '*') //the user wants to change the password
		{
			First_Run(); // to enter the new password and send it to MC2 to save it
		}
		else if (key == '%') // the user wants to open the door
		{
			Open_gate(Config_Ptr);
		}
	}
	else /*This means that the user has entered the password wrong for 3 times*/
	{
		return_flag=0; //to work correct the next time
	}

}

int main(void){

	uint8 First_Run_Flag=0;
	/*To check whether the one run code is done before or not
	 *MC2 read the address where the Initialized flag is declared (first run)
	 *and put it in this flag then send it to MC1. If this is equal to the
	 *Initialized flag, then the one run code was done so we will go to the Main Options*/

	Timer_Config Timer_Config = {timer0,OVERFLOW,CPU_1024,0,0}; //input to the timer structure


	/*Set the call back function pointer in the timer driver
	 to the function that calculate seconds*/
	Timer0_Overflow_CallBack(Cal_Time);


	UART_Config UART_Config = {disabled,bit1,bit8,9600};////input to the UART structure


	LCD_init();
	UART_init(& UART_Config);




	while(1){

		/*M1 sends to M2 that it's ready
		then ,M2 reads the address of the initialized flag and send it to M1
		to check if it's first run or not*/
		UART_sendByte(M1_Ready);
		First_Run_Flag= UART_recieveByte();


		if(First_Run_Flag != Init_Flag )
		{
			//if they are not equal so it's the first run
			First_Run();
		}

		else
		{//if they are  equal so the first run is done
			Main_Options(& Timer_Config);
		}


	}
}
