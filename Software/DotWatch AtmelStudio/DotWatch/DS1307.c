/*
 * DS1307.c
 *
 * Created: 03.01.2017 17:38:04
 *  Author: Oleksandr
 */ 

 #include "DS1307.h"


//time_type currTimeBCD = {0, 0, 0};



uint8_t RTC_UintToBCD(uint8_t uintValue)
{
	uint8_t result = 0;
	uint8_t temp = 0;
	
	result = uintValue%10;
	temp = uintValue/10;
	if (temp >= 10)
	{
		temp = 0;
	}
	result |= (temp)<<4;

	return result;
}



uint8_t RTC_BCDToUint(uint8_t BCDValue)
{
	uint8_t result = 0;
	result = (BCDValue & 0x0F) + (BCDValue>>4)*10;
	return result;
}



void RTC_WriteByte(uint8_t addr, uint8_t byte)
{
	i2c_start_wait(DS1307_ADDR+I2C_WRITE);
	i2c_write(addr);
	i2c_write(byte);
	i2c_stop();
}



uint8_t RTC_ReadByte(uint8_t addr)
{
	uint8_t recByte = 0;
	i2c_start_wait(DS1307_ADDR+I2C_WRITE);
	i2c_write(addr);
	i2c_rep_start(DS1307_ADDR+I2C_READ);
	recByte = i2c_readNak();

	return recByte;
}



 void RTC_TimeGet(time_type *timeValue)
 {
	 i2c_start_wait(DS1307_ADDR+I2C_WRITE);
	 i2c_write(RTC_REG_SECONDS);
	 i2c_rep_start(DS1307_ADDR+I2C_READ);
	 timeValue->Seconds =	i2c_readAck() & 0x7F;
	 timeValue->Minutes =	i2c_readAck();
	 timeValue->Hours =		i2c_readNak() & 0x3F;
 }



 void RTC_TimeSet(time_type *timeValue)
 {
	i2c_start_wait(DS1307_ADDR+I2C_WRITE);
	i2c_write(RTC_REG_SECONDS);
	i2c_write(timeValue->Seconds);
	i2c_write(timeValue->Minutes);
	i2c_write(timeValue->Hours);
	i2c_stop();
 }


 uint8_t RTC_Init(void)
 {
	uint8_t temp = 0;
	time_type timeVal;

	RTC_WriteByte(RTC_REG_CONTROL, 0x10);	//enable 1hz generator

	temp = RTC_ReadByte(RTC_REG_SECONDS);

	if ((temp & 0x80) != 0)//rtc stopped
	{
		timeVal.Seconds = RTC_UintToBCD(RTC_DEFAULT_SECONDS);
		timeVal.Minutes = RTC_UintToBCD(RTC_DEFAULT_MINUTES);
		timeVal.Hours = RTC_UintToBCD(RTC_DEFAULT_HOURS);

		RTC_TimeSet(&timeVal);
		return RTC_NEWINIT;
	}

	return RTC_NOINIT;
 }

