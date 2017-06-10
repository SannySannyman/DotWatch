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

#define RTC_DEFAULT_HOURS	12
#define RTC_DEFAULT_MINUTES	13
#define RTC_DEFAULT_SECONDS 14

#define RTC_REG_SECONDS		0x00
#define RTC_REG_MINUTES		0x01
#define RTC_REG_HOURS		0x02
#define RTC_REG_DAY			0x03
#define RTC_REG_DATE		0x04
#define RTC_REG_MONTH		0x05
#define RTC_REG_YEAR		0x06
#define RTC_REG_CONTROL		0x07

#define RTC_SECONDS_PER_MINUTE     60u
#define RTC_MINUTES_PER_HOUR       60u
#define RTC_HOURS_PER_DAY          24u
#define RTC_SECONDS_PER_HOUR       (60u*60u)
#define RTC_SECONDS_PER_DAY        (24ul*60ul*60ul)

#define RTC_SECONDS_MAX    (RTC_SECONDS_PER_MINUTE-1u)
#define RTC_MINUTES_MAX    (RTC_MINUTES_PER_HOUR-1u)
#define RTC_HOURS_MAX      (RTC_HOURS_PER_DAY-1u)

#define BCD_ONES_MASK		(0x0Fu)
#define BCD_TENS_MASK		(0xF0u)

#define BCD_ONES_SHIFT		(0u)
#define BCD_TENS_SHIFT		(4u)

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