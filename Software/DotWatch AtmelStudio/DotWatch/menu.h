/*
 * menu.h
 *
 * Created: 26.04.2017 23:39:53
 *  Author: Oleksandr
 */ 


#ifndef MENU_H_
#define MENU_H_

#include "common.h"
#include "DS1307.h"
#include "dot_display.h"

#define UPD_US		0
#define UPD_FORCE	1

typedef enum 
{
	MODE_NORMAL,
	MODE_EDIT_HOURS,
	MODE_EDIT_MINS,
	MODE_EDIT_SEC,
	MODE_STATENUM
}mode_t;


void MenuInit(void);
void DisplayUpdate(uint8_t updMode);
void DisplayMenuUpdate(uint8_t btnStatus, uint16_t btnHoldCnt);
void DisplayMaskToggle(void);


#endif /* MENU_H_ */