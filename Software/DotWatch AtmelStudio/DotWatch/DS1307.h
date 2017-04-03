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

#define RTC_DEFAULT_HOURS	20
#define RTC_DEFAULT_MINUTES	13
#define RTC_DEFAULT_SECONDS 55

#define RTC_REG_SECONDS		0x00
#define RTC_REG_MINUTES		0x01
#define RTC_REG_HOURS		0x02
#define RTC_REG_DAY			0x03
#define RTC_REG_DATE		0x04
#define RTC_REG_MONTH		0x05
#define RTC_REG_YEAR		0x06
#define RTC_REG_CONTROL		0x07



typedef struct 
{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
}time_type;

enum
{
	RTC_NEWINIT = 0x00,
	RTC_NOINIT
};


uint8_t RTC_UintToBCD(uint8_t uintValue);
uint8_t RTC_BCDToUint(uint8_t BCDValue);
void RTC_WriteByte(uint8_t addr, uint8_t byte);
uint8_t RTC_ReadByte(uint8_t addr);
void RTC_TimeGet(time_type *timeValue);
void RTC_TimeSet(time_type *timeValue);
uint8_t RTC_Init(void);


#endif /* DS1307_H_ */