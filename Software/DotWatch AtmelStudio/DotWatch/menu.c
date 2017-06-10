/*
 * menu.c
 *
 * Created: 26.04.2017 23:40:09
 *  Author: Oleksandr
 */ 
 #include "menu.h"

 uint8_t editValUpd = FALSE;
 uint8_t menuMode = MODE_NORMAL;

 time_type newTimeBCD = {0, 0, 0};
 time_type timeBCD = {0, 0, 0};
 uint16_t displayMask = DISPLAY_ALL_MASK;


 uint8_t isEditMode(void)
 {
	if (menuMode == MODE_NORMAL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	
 }

 void BCDaddOne(uint8_t *var, uint8_t varMax)
 {
	uint8_t temp = 0;

	temp = RTC_BCDToUint(*var);

	temp += 1;

	if (temp >= varMax)
	{
		temp = 0;
	}

	*var = RTC_UintToBCD(temp);
 }


 void DisplayUpdate(uint8_t updMode)
 {
	switch(menuMode)
	{
		case MODE_NORMAL:
			RTC_TimeGet(&timeBCD);
			if ((RTC_BCDToUint(timeBCD.Seconds)%5 == 0)||(updMode == UPD_FORCE))
			{
				genDotBuffers(timeBCD.Hours, timeBCD.Minutes);
				dotBuffUpdate(DISPLAY_ALL_MASK);
			}
			break;

		default: 
			dotBuffUpdate(displayMask);
			break;
	}
 }
 
  void MenuInit(void)
  {
	  DisplayUpdate(UPD_FORCE);
  }


 void DisplayMenuUpdate(uint8_t btnStatus, uint16_t btnHoldCnt)
 {

	if (check_bit(btnStatus, STATUS_MODE_BTN))
	{
		switch(menuMode)
		{
			case MODE_NORMAL:
				if (btnHoldCnt >= BTN_EDIT_HOLD)
				{
					//rtc save time
					RTC_TimeGet(&newTimeBCD);
					menuMode++;
				}
			break;

			default:
				if (btnHoldCnt >= BTN_EDIT_HOLD)
				{
					RTC_TimeSet(&newTimeBCD);
				}
				menuMode++;
				break;
		}

		if (menuMode >= MODE_STATENUM)
		{
			menuMode = MODE_NORMAL;
			//save new value
			//RTC_TimeSet(&newTimeBCD);
		}
	}

	if (check_bit(btnStatus, STATUS_INC_BTN))
	{
		switch(menuMode)
		{
			case MODE_NORMAL:
			//
			break;

			case MODE_EDIT_HOURS:
			BCDaddOne(&newTimeBCD.Hours, RTC_HOURS_PER_DAY);
			break;

			case MODE_EDIT_MINS:
			BCDaddOne(&newTimeBCD.Minutes, RTC_MINUTES_PER_HOUR);
			break;

			case MODE_EDIT_SEC:
			BCDaddOne(&newTimeBCD.Seconds, RTC_SECONDS_PER_MINUTE);
			break;
		}
	}
	

	switch(menuMode)
	{
		case MODE_NORMAL:
			break;

		case MODE_EDIT_HOURS:
			genDotBuffers(EDIT_IND_HOURS, newTimeBCD.Hours);
			break;

		case MODE_EDIT_MINS:
			genDotBuffers(EDIT_IND_MINS, newTimeBCD.Minutes);
			break;

		case MODE_EDIT_SEC:
			genDotBuffers(EDIT_IND_SEC, newTimeBCD.Seconds);
			break;
	}

	DisplayUpdate(UPD_FORCE);
 }

 void DisplayMaskToggle(void)
 {
	static uint8_t cnt = 0;
	cnt++;
	if (cnt % 2 == 0)
	{
		displayMask = DISPLAY_MIN_MASK;
	}
	else
	{
		displayMask = DISPLAY_ALL_MASK;
	}
 }