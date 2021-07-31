
#include "UART.h"
uint8 flag_9bits=0; // to help us handling 9bits transmission

// ehhhhh daaa b2aaa
//#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 8UL))) - 1)


//Functions Definitions


void UART_init(const UART_Config * Config_Ptr)
/*in this function , we want to
 * set the parity
 * chosse stop bits
 * choose number of bits (size of data sent)
 * set the required baudrate */
{
	uint32 baudrate;
	uint8 BAUD_PRESCALE; //to put in it the baud_prescale according to the equation

	UCSRA = (1<<U2X);// for double transmission speed

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/
	UCSRB = (1<<RXEN) | (1<<TXEN);

	if(Config_Ptr->size ==7 )//9bit data
	{
		UCSRB |= (1<<UCSZ2); // 9-bits data
		flag_9bits =1;
	}

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = 00 Disable parity bit
	 * USBS    = 0 One stop bit
	 * UCSZ1:0 = 11 For 8-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/
	UCSRC = (1<<URSEL) ;

	switch(Config_Ptr->parity)// to choose the parity (even or odd modes only)
	{
	case 2:
		UCSRC |= (1<<UPM1); //Even parity
		break;
	case 3:
		UCSRC |= (1<<UPM0) | (1<<UPM1); //Odd parity
		break;
	}
	switch(Config_Ptr->stop)// to choose number of stop bits
	{
	case 1:
		UCSRC |= (1<<USBS); // 2bits to stop
	}

	//put the chosen size in the 2nd,3rd and 4th bits in UCSRC
	UCSRC = (UCSRC & 0xF9) | ((Config_Ptr->size)<<1);

	//First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH
	baudrate = Config_Ptr->BaudRate;
	BAUD_PRESCALE= ((F_CPU / (baudrate * 8UL)) - 1);
	UBRRH = BAUD_PRESCALE>>8;
	UBRRL = BAUD_PRESCALE;
}

void UART_sendByte(const uint16 data)
{   uint8 bit_9=0;   // to get the bit number 9 of data
/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for
 * transmitting a new byte so wait until this flag is set to one */
while(BIT_IS_CLEAR(UCSRA,UDRE)){}
/* Put the required data in the UDR register and it also clear the UDRE flag as
 * the UDR register is not empty now */
if(flag_9bits==1)
{
	bit_9= data & (1<<8);  // to get the bit number 9 of data
	UCSRB = (UCSRB & 0xFE) | bit_9; //Put this bit in TXB8
}
UDR = data;
}

uint16 UART_recieveByte(void)
{     uint8 bit_9=0;   // to get the bit number 9 of data
/* RXC flag is set when the UART receive data so wait until this
 * flag is set to one */
while(BIT_IS_CLEAR(UCSRA,RXC)){}
/* Read the received data from the Rx buffer (UDR) and the RXC flag
	   will be cleared after read this data */
bit_9 = (UCSRB & 1); //Read the bit number 9
return (UDR | (bit_9<<8));
}

void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}
	 *******************************************************************/
}

void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_recieveByte();
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}
	Str[i] = '\0';
}
