
#include "i2c.h"
#include "eeprom.h"
// FUNCTIONS                             *

/*
 * Function that initialize the EEPROM by initializing I2C
 * the required prescaler, devision factor and slave address
 */
void EEPROM_init(const TWI_Config * Config_Ptr)
{

	TWI_init(Config_Ptr); /*Initialize I2C*/
}

//To write the required data in the given address
uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data)
{
	/* Send the Start Bit */
	TWI_start();
	if (TWI_getStatus() != TW_START)
		return ERROR;

	/* Send the device address, we need to get A8 A9 A10 address bits from the
	 * memory location address and R/W=0 (write) */
	TWI_write((uint8)(((u16addr & 0x0700)>>7)) | 0xA0);
	if (TWI_getStatus() != TW_MT_SLA_W_ACK)
		return ERROR;

	/* Send the required memory location address */
	TWI_write((uint8)(u16addr));
	if (TWI_getStatus() != TW_MT_DATA_ACK)
		return ERROR;

	/* write byte to eeprom */
	TWI_write(u8data);
	if (TWI_getStatus() != TW_MT_DATA_ACK)
		return ERROR;

	/* Send the Stop Bit */
	TWI_stop();

	return SUCCESS;
}

// to read the data from the given address
uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data)
{
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;

    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=0 (write) */
    TWI_write((uint8)((0xA0) | ((u16addr & 0x0700)>>7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR;

    /* Send the required memory location address */
    TWI_write((uint8)(u16addr));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* Send the Repeated Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_REP_START)
        return ERROR;

    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=1 (Read) */
    TWI_write((uint8)((0xA0) | ((u16addr & 0x0700)>>7) | 1));
    if (TWI_getStatus() != TW_MT_SLA_R_ACK)
        return ERROR;

    /* Read Byte from Memory without send ACK */
    *u8data = TWI_readWithNACK();
    if (TWI_getStatus() != TW_MR_DATA_NACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
    return SUCCESS;
}
