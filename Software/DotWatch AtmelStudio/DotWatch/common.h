/*
 * common.h
 *
 * Created: 26.09.2016 18:09:31
 *  Author: Oleksandr
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#include <avr/io.h>
#include "bits.h"

#define TRUE	1
#define FALSE	0

#define D_CLK_DDR	DDRB
#define D_CLK_PORT	PORTB
#define D_CLK_PIN	PINB5

#define D_DATA_DDR	DDRB
#define D_DATA_PORT	PORTB
#define D_DATA_PIN	PINB3

#define D_UPD_DDR	DDRB
#define D_UPD_PORT	PORTB
#define D_UPD_PIN	PINB2

#define MODE_BTN_DDR	DDRC
#define MODE_BTN_PORT	PORTC
#define MODE_BTN_PIN	PINC
#define MODE_BTN_PIN_N	PINC3

#define INC_BTN_DDR		DDRC
#define INC_BTN_PORT	PORTC
#define INC_BTN_PIN		PINC
#define INC_BTN_PIN_N	PINC2

#define STATUS_MODE_BTN		0
#define STATUS_INC_BTN		1
#define BTN_EDIT_HOLD		20

typedef enum
{
	TIME_GET_IDLE,
	TIME_GET_START_SYM,
	TIME_GET_HOURS10,
	TIME_GET_HOURS1,
	TIME_GET_SEPARATOR1,
	TIME_GET_MINUTES10,
	TIME_GET_MINUTES1,
	TIME_GET_SEPARATOR2,
	TIME_GET_SECONDS10,
	TIME_GET_SECONDS1,
	TIME_GET_STATE_NUM
}timeGetState_type;


enum 
{
	EDIT_IND_VAL = 0xA0,
	EDIT_IND_HOURS,
	EDIT_IND_MINS,
	EDIT_IND_SEC
};


#endif /* COMMON_H_ */