#ifndef I2C_H_
#define I2C_H_

#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"

//types' declaration to I2C config.

typedef enum
{
	F_CPU_CLOCK,F_CPU_CLOCK_4,F_CPU_CLOCK_16,F_CPU_CLOCK_64
}TWI_Prescaler;

typedef struct
{
	TWI_Prescaler prescaler;
	uint8 s_devision_factor; // from user
	uint8 s_slave_address; //Take the address as it should be known if it became slave
}TWI_Config;



 //Definitions

// I2C Status Bits in the TWSR Register
#define TW_START         0x08 // start has been sent
#define TW_REP_START     0x10 // repeated start
#define TW_MT_SLA_W_ACK  0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
#define TW_MT_SLA_R_ACK  0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
#define TW_MT_DATA_ACK   0x28 // Master transmit data and ACK has been received from Slave.
#define TW_MR_DATA_ACK   0x50 // Master received data and send ACK to slave
#define TW_MR_DATA_NACK  0x58 // Master received data but doesn't send ACK to slave


//functions

// Function that initialize the I2C (required prescaler, devision factor and slave address)
 // the registers are initialized

void TWI_init(const TWI_Config* Config_Ptr);

void TWI_start(void);

void TWI_stop(void);

void TWI_write(uint8 data);

uint8 TWI_readWithACK(void);


uint8 TWI_readWithNACK(void);

uint8 TWI_getStatus(void);

#endif
