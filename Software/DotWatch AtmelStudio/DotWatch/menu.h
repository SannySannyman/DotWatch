/*
 * menu.h
 *
 * Created: 26.04.2017 23:39:53
 *  Author: Oleksandr
 */ 


#ifndef MENU_H_
#define MENU_H_

#include "common.h"

enum 
{
	EDIT_OFF,
	EDIT_HOUR_TENS,
	EDIT_HOUR_ONES,
	EDIT_MIN_TENS,
	EDIT_MIN_ONES,
	
	EDIT_STATENUM
};


uint8_t isEditMode(void);


#endif /* MENU_H_ */