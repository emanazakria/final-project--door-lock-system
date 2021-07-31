/*
 * micro_config.h
 *
 *  Created on: Nov 6, 2020
 *      Author: User
 */

#ifndef MICRO_CONFIG_H_
#define MICRO_CONFIG_H_

#ifndef F_CPU
#define F_CPU 8000000UL //8MHz Clock frequency
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#endif /* MICRO_CONFIG_H_ */
