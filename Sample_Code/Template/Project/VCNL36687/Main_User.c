
#include "string.h"
#include "Main_User.h"

#include "I2C_Functions.h"
//#include "MS51_16K.h"
#include "i2c_conf.h"
//Global Variables

int I2C_Bus = 0;

//Variables for Offset Value
//int CalibValue = 0;
//int AverageCount = 10; //Change the average count to the needed number of offset measurement

void HAL_Delay(unsigned int timing)
{
//	Timer2_Delay(SYS_CLOCK,16,1,timing*1000);
	Timer_Delay(SYS_CLOCK,timing,1000);
}

int WriteI2C_Bus(TransferData *pData)
{
	I2Cx_Write_Multi_ToSlave(pData->Slave_Address , pData->RegisterAddress , pData->WData , 2);
	HAL_Delay(5);
	return TRUE;
}

int ReadI2C_Bus (TransferData *pData)
{
	I2Cx_Read_Multi_FromSlave(pData->Slave_Address , pData->RegisterAddress , pData->RData , 2);
	HAL_Delay(5);	
	return TRUE;
}

unsigned int VCNL36687_GET_PS_Data(void)
{
	TransferData VCNL36687_Data;
	VCNL36687_Data.Slave_Address = VCNL36687_SlaveAddress;
	VCNL36687_Data.RegisterAddress = VCNL36687_PS_DATA;
    VCNL36687_Data.Select_I2C_Bus = I2C_Bus;
	ReadI2C_Bus(&VCNL36687_Data);
	return (VCNL36687_Data.RData[1]<<8|VCNL36687_Data.RData[0]);
}

void VCNL36687_SET_THRESHOLD(unsigned char reg , unsigned char value_lsb, unsigned char value_msb)
{

	TransferData VCNL36687_Data;
	VCNL36687_Data.Slave_Address = VCNL36687_SlaveAddress;
	VCNL36687_Data.RegisterAddress = reg;
    VCNL36687_Data.Select_I2C_Bus = I2C_Bus;
	VCNL36687_Data.WData[0] = value_lsb;
	VCNL36687_Data.WData[1] = value_msb;
	WriteI2C_Bus(&VCNL36687_Data);
}

void VCNL36687_SET_CONF5(unsigned char conf5_L, unsigned char conf5_M)
{

	TransferData VCNL36687_Data;
	VCNL36687_Data.Slave_Address = VCNL36687_SlaveAddress;
	VCNL36687_Data.RegisterAddress = VCNL36687_PS_CONF5;
    VCNL36687_Data.Select_I2C_Bus = I2C_Bus;
	VCNL36687_Data.WData[0] = conf5_L;
	VCNL36687_Data.WData[1] = conf5_M;
	WriteI2C_Bus(&VCNL36687_Data);
}

void VCNL36687_SET_CONF1(unsigned char conf1, unsigned char conf2)
{

	TransferData VCNL36687_Data;
	VCNL36687_Data.Slave_Address = VCNL36687_SlaveAddress;
	VCNL36687_Data.RegisterAddress = VCNL36687_PS_CONF1;
    VCNL36687_Data.Select_I2C_Bus = I2C_Bus;
	VCNL36687_Data.WData[0] = conf1;
	VCNL36687_Data.WData[1] = conf2;
	WriteI2C_Bus(&VCNL36687_Data);
}

void VCNL36687_SET_VCSEL(unsigned char conf3, unsigned char conf4)
{

	TransferData VCNL36687_Data;
	VCNL36687_Data.Slave_Address = VCNL36687_SlaveAddress;
	VCNL36687_Data.RegisterAddress = VCNL36687_PS_CONF4;
    VCNL36687_Data.Select_I2C_Bus = I2C_Bus;
	VCNL36687_Data.WData[0] = conf3;
	VCNL36687_Data.WData[1] = conf4;
	WriteI2C_Bus(&VCNL36687_Data);
}


void INIT_VCNL36687(void)
{
	#if 1
	VCNL36687_SET_VCSEL(VCNL36687_PS_Auto , VCNL36687_VCSEL_I_20mA );
	
	VCNL36687_SET_CONF1(VCNL36687_PS_PERIOD_8ms | VCNL36687_PS_ON , VCNL36687_PS_8T | VCNL36687_PS_MPS_1 | VCNL36687_PS_ITB_25);
	VCNL36687_SET_CONF5(VCNL36887_POR_S , 0x00);
	#else	//https://github.com/mehtajainam/VCNL36687
	VCNL36687_SET_VCSEL(VCNL36687_PS_Auto , VCNL36687_PS_HD_EN | VCNL36687_VCSEL_I_20mA );
	
	VCNL36687_SET_CONF1(VCNL36687_PS_PERIOD_32ms | VCNL36687_PS_ON , VCNL36687_PS_2T | VCNL36687_PS_MPS_1 | VCNL36687_PS_ITB_25);
	VCNL36687_SET_CONF5(VCNL36887_POR_S , 0x00);
	
	VCNL36687_SET_THRESHOLD(VCNL36687_PS_THDL , 0x88 , 0x13 );
	VCNL36687_SET_THRESHOLD(VCNL36687_PS_THDH , 0xE0 , 0x2E );	
	#endif
	
}

//Print Function
void PRINT_VCNL36687(void)
{
	unsigned int result = 0;
	
	result = VCNL36687_GET_PS_Data();

	if ((result == 0xC1C1) || (result == 0x588))
	{
		printf("*\r\n");
	}
	else
	{
		printf("H :0x%2BX,L : 0x%2BX(dec: %5u)\r\n" , HIBYTE(result),LOBYTE(result),(unsigned int)result );
//		printf("(dec: %5u)\r\n" , (unsigned int)result );
	}
	
}


