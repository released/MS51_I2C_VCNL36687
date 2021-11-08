/*
 * I2C_Functions.h
 *
 * Created  : 23 July 2021
 * Modified : 4 August 2021
 * Author   : HWanyusof
 * Version	: 1.1
 */

//#pragma once

#include <stdio.h>
#include <stdlib.h>
//#include "typedefinition.h"
#include "MS51_16K.h"

//Struct TransferData Member Definition
typedef struct 
{
	uint8_t RegisterAddress;
	uint8_t WData[2];
	uint8_t RData[2];
	uint8_t length;
	uint8_t Slave_Address;
	uint8_t Select_I2C_Bus;
}TransferData; //Struct variables will be declared separately in Sensor API and I2C_Functions.cpp/c

//Function Prototypes For I2C_Functions.cpp/c
void HAL_Delay(uint16_t timing);
int ReadI2C_Bus(TransferData *pData);
int WriteI2C_Bus(TransferData *pData);

