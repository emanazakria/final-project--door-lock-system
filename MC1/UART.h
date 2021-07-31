#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

//types' declaration to UART config.

typedef enum {
disabled, reversed , even , odd
}UART_Parity;

typedef enum {
	bit1,bit2
}UART_Stopbit;

typedef enum {
	bit5,bit6,bit7,bit8,reversed1,reversed2,reversed3,bit9
}UART_Datasize;

typedef struct {
	UART_Parity parity;
	UART_Stopbit stop;
	UART_Datasize size;
	uint32 BaudRate; //take it from the user
}UART_Config;




 //Functions
void UART_init(const UART_Config * Config_Ptr);

void UART_sendByte(const uint16 data);

uint16 UART_recieveByte(void);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
