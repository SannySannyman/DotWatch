/*
 * display.c
 *
 * Created: 03.01.2017 16:09:45
 *  Author: Oleksandr
 */ 

 #include "dot_display.h"

 volatile uint8_t displayBuff[DISPLAY_BUFF_LEN];
 uint8_t displayBuffTemp[DISPLAY_BUFF_LEN];
 const uint8_t indValue = 0b00010101;

 void displayInit(void)
 {
	set_bit(D_UPD_DDR, D_UPD_PIN);
	set_bit(D_CLK_DDR, D_CLK_PIN);
	set_bit(D_DATA_DDR, D_DATA_PIN);
	set_bit5(SPCR, SPIE, SPE, MSTR, SPR1, SPR0);	//SPI interrupt, SPI en, master, clk/128
	SPDR = 0x00;
 }

 ISR(SPI_STC_vect)
 {
	 static uint8_t cnt = 0;

	 SPDR = displayBuff[cnt];

	 if (cnt%2 == 0)
	 {
		 set_bit(D_UPD_PORT, D_UPD_PIN);
		 unset_bit(D_UPD_PORT, D_UPD_PIN);
	 }

	 cnt++;
	 if (cnt >= DISPLAY_BUFF_LEN)
	 {
		 cnt = 0;
	 }
 }


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

 void genDotBuffers(uint8_t BCDhours, uint8_t BCDmins)
 {
	 uint16_t dotBuff[TIME_EN_N];
	 uint8_t temp = 0;
	 uint8_t rowCnt = 0;

	 if (BCDhours < EDIT_IND_VAL)
	 {
		 dotBuff[TIME_HOUR_O] = randGenDots((BCDhours & 0x0F), 9);
		 dotBuff[TIME_HOUR_T] = randGenDots((BCDhours >> 4), 6);
	 }
	 else
	 {
		dotBuff[TIME_HOUR_O] = 0;
		dotBuff[TIME_HOUR_T] = indValue>>((2 - (BCDhours - EDIT_IND_HOURS))*2);
	 }


	 if (BCDmins < EDIT_IND_VAL)
	 {
		dotBuff[TIME_MIN_O] = randGenDots((BCDmins & 0x0F), 9);
		dotBuff[TIME_MIN_T] = randGenDots((BCDmins >> 4), 6);
	 }
	 else
	 {
		dotBuff[TIME_MIN_O] = 0;
		dotBuff[TIME_MIN_T] = indValue>>((2 - (BCDmins - EDIT_IND_HOURS))*2);
	 }
	 

	for (rowCnt = 0; rowCnt < DISPLAY_ROW_N; rowCnt++)
	{
		temp = rowCnt * 2;

		displayBuffTemp[temp] =
		//(1<<(rowCnt+2)) |
		((dotBuff[TIME_HOUR_T] >> (DISPLAY_HOUR_T_ROW_WIDTH * rowCnt))	&
		DISPLAY_HOUR_T_N_MASK);

		displayBuffTemp[temp+1] =
		(((dotBuff[TIME_HOUR_O] >> (DISPLAY_HOUR_O_ROW_WIDTH * rowCnt))	&
		DISPLAY_HOUR_O_N_MASK)	<< DISPLAY_HOUR_O_COLUMN_START)		|
		(((dotBuff[TIME_MIN_T] >> (DISPLAY_MIN_T_ROW_WIDTH * rowCnt))		&
		DISPLAY_MIN_T_N_MASK)	<< DISPLAY_MIN_T_COLUMN_START)		|
		(((dotBuff[TIME_MIN_O] >> (DISPLAY_MIN_O_ROW_WIDTH * rowCnt))		&
		DISPLAY_MIN_O_N_MASK)	<< DISPLAY_MIN_O_COLUMN_START) ;
	}

 }

 void dotBuffUpdate(uint16_t enableMask)
 {
	uint8_t cnt = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for(cnt = 0; cnt < DISPLAY_BUFF_LEN; cnt++)
		{
			displayBuff[cnt] = displayBuffTemp[cnt];

			if(cnt % 2 == 0)
			{
				displayBuff[cnt] &= (uint8_t)(enableMask>>8);
				displayBuff[cnt] |= (1<<(cnt/2+2));
			}
			else
			{
				displayBuff[cnt] &= (uint8_t)enableMask;
			}
		}

	}
 }
