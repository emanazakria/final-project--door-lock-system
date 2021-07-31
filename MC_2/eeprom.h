
#ifndef EX_EEPROM_H_
#define EX_EEPROM_H_

#include "std_types.h"
#include"i2c.h"
// Definitions

#define ERROR 0
#define SUCCESS 1

//  FUNCTIONS DECLARATIONS                           *

/*Function that initialize the EEPROM by initializing I2C
 * THErequired prescaler, devision factor and slave address */
 void EEPROM_init(const TWI_Config * Config_Ptr);

uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data);

uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data);

#endif /* EX_EEPROM_H_ */
