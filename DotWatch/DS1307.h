/*
 * DS1307.h
 *
 * Created: 03.01.2017 17:38:21
 *  Author: Oleksandr
 */ 


#ifndef DS1307_H_
#define DS1307_H_

#include "i2cmaster.h"
#include "common.h"

#define DS1307_ADDR 0xD0

typedef struct 
{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
}time_type;


#endif /* DS1307_H_ */