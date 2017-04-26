/*
 * display.c
 *
 * Created: 03.01.2017 16:09:45
 *  Author: Oleksandr
 */ 

 #include "dot_display.h"

 uint16_t randGenDots(uint8_t nDots, uint8_t nMax)
 {
	 uint16_t genBuff = 0;
	 uint8_t randN = 0;
	 uint8_t invertMode = FALSE;

	 if (nDots > nMax)
	 {
		 nDots = nMax;
	 }

	 if(nDots > nMax/2)
	 {
		 invertMode = TRUE;
		 nDots = nMax - nDots;
	 }

	 for (uint8_t i = 0; i < nDots; i++)
	 {
		 randN = rand() % nMax;

		 for (uint8_t tryCnt = 0; tryCnt < nMax; tryCnt++)
		 {
			 if ((genBuff & (1<<randN)) == 0)
			 {
				 genBuff |= 1<<randN;
				 break;
			 }
			 else
			 {
				 randN++;
				 if (randN >= nMax)
				 {
					 randN = 0;
				 }
			 }
		 }
	 }

	 if (invertMode == TRUE)
	 {
		 genBuff ^= ~(0xFFFF<<nMax);
	 }

	 return genBuff;
 }

 void genDotBuffers(uint8_t BCDhours, uint8_t BCDmins, uint8_t *displayBuffPtr)
 {
	 uint16_t dotBuff[TIME_EN_N];
	 uint8_t temp = 0;
	 uint8_t buffCnt = 0;

	 dotBuff[TIME_MIN_O] = randGenDots((BCDmins & 0x0F), 9);
	 dotBuff[TIME_MIN_T] = randGenDots((BCDmins >> 4), 6);
	 
	 dotBuff[TIME_HOUR_O] = randGenDots((BCDhours & 0x0F), 9);
	 dotBuff[TIME_HOUR_T] = randGenDots((BCDhours >> 4), 6);

	for (buffCnt = 0; buffCnt < DISPLAY_BUFF_LEN/2; buffCnt++)
	{
		temp = buffCnt * 2;

		displayBuffPtr[temp] =
		(1<<(buffCnt+2)) |
		((dotBuff[TIME_HOUR_T] >> (DISPLAY_HOUR_T_ROW_WIDTH * buffCnt))	&
		DISPLAY_HOUR_T_N_MASK);

		displayBuffPtr[temp+1] =
		(((dotBuff[TIME_HOUR_O] >> (DISPLAY_HOUR_O_ROW_WIDTH * buffCnt))	&
		DISPLAY_HOUR_O_N_MASK)	<< DISPLAY_HOUR_O_COLUMN_START)		|
		(((dotBuff[TIME_MIN_T] >> (DISPLAY_MIN_T_ROW_WIDTH * buffCnt))		&
		DISPLAY_MIN_T_N_MASK)	<< DISPLAY_MIN_T_COLUMN_START)		|
		(((dotBuff[TIME_MIN_O] >> (DISPLAY_MIN_O_ROW_WIDTH * buffCnt))		&
		DISPLAY_MIN_O_N_MASK)	<< DISPLAY_MIN_O_COLUMN_START) ;
	}

 }

 void dotBuffUpdate(const uint8_t *displayBuffPtr, uint8_t enable)
 {
	uint8_t buffCnt = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(buffCnt = 0; buffCnt < DISPLAY_BUFF_LEN; buffCnt++)
		{
			displayBuff[buffCnt] = (enable == FALSE)?(0):(displayBuffPtr[buffCnt]);
		}

	}
 }
