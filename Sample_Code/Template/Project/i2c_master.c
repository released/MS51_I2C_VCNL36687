
#include "i2c_conf.h"

//#include <stdio.h>
//#include "N76E003.h"
//#include "SFR_Macro.h"
//#include "Common.h"


#include "MS51_16K.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

volatile unsigned char g_u8DeviceAddr_m;
volatile unsigned char g_u8DataLen_m;
volatile unsigned char rawlenth;
volatile unsigned char g_au8Reg;
volatile unsigned char g_u8EndFlag = 0;
volatile unsigned char *g_au8Buffer;

typedef void (*I2C_FUNC)(unsigned long u32Status);

volatile I2C_FUNC I2Cx_Master_HandlerFn = NULL;

bit I2C_Reset_Flag;


/*---------------------------------------------------------------------------------------------------------*/
/*  I2C IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

void I2C_start(void)
{
    signed int t = I2C_timeout_count;
    set_STA;                               
    clr_SI;
    while((SI == 0) && (t > 0))
    {
       t--;
    };    
}

void I2C_stop(void)
{
    signed int t = I2C_timeout_count;
    clr_SI;
    set_STO;
    while((STO == 1) && (t > 0))
    {
        t--;
    };    
}

unsigned char I2C_read(unsigned char ack_mode)
{
    signed int t = I2C_timeout_count;
    unsigned char value = 0x00;
    set_AA;                            
    clr_SI;
    while((SI == 0) && (t > 0))
    {
        t--;
    };      
    value = I2DAT;
    if(ack_mode == I2C_NACK)
    {
        t = I2C_timeout_count;
        clr_AA;  
        clr_SI;
        while((SI == 0) && (t > 0))
        {
            t--;
        };          
    }
    return value;
}


void I2C_write(unsigned char value)
{
    signed int t = I2C_timeout_count;
    I2DAT = value;
    clr_STA;          
    clr_SI;
    while((SI == 0) && (t > 0))
    {
        t--;
    }; 
}


unsigned char CRC_Get(unsigned char* array , unsigned int len)
{
    // Return the CRC8 value for the given byte array (data), given length
    unsigned char x, crc = 0xFF;
    while (len--) 
	{
        x = crc >> 4 ^ *array++;
        x ^= (x >> 2);
        crc = (unsigned char)(((unsigned char)(crc << 4)) ^ ((unsigned char)(x << 6)) ^ ((unsigned char)(x << 3)) ^ (unsigned char)x);
    }
    return crc;
}

void I2C_SI_Check(void)
{
    printf("I2C Write error \r\n");

    if (I2STAT == 0x00)
    {
        I2C_Reset_Flag = 1;
        set_I2CON_STO;
        SI = 0;
        if(SI)
        {
            clr_I2CON_I2CEN;
            set_I2CON_I2CEN;
        }   
    }  
}

void I2Cx_Master_LOG(unsigned long u32Status)
{
	#if defined (DEBUG_LOG_MASTER_LV1)
    printf("%s  : 0x%2x \r\n", __FUNCTION__ , u32Status);

	#else
	UNUSED_VARIABLE(u32Status);
	
	#endif
}


/*
	Solution: 
	¡V  Send a STOP condition to I2C bus 
	¡V  If the STOP condition is invalid, disable the I2C bus and then restart the communication. 

*/
void I2Cx_Master_BusError(void)
{
	while(SI != 0) 
	{ 
		if (I2STAT == 0x00) 
		  { 
			set_STO;       // Check bus status if bus error¡Afirst send stop 
		  } 
		clr_SI; 
		if(SI != 0)       // If SI still keep 1  
		{ 
			clr_I2CEN;      // please first disable I2C. 
			set_I2CEN ;      // Then enable I2C for clear SI. 
			clr_SI; 
			clr_I2CEN;      // At last disable I2C for next a new transfer 
		}   
	} 
}

void I2Cx_Master_IRQHandler(void) interrupt 6
{
    unsigned int t = I2C_timeout_count;
	unsigned char flag = 0;
    unsigned long u32Status;
	
    u32Status = I2STAT;

	#if 1
    if ( I2TOC &= SET_BIT0)
    {
        I2Cx_Master_BusError();    
    }
	#else
    while((SI == 0) && (t > 0))
    {
       t--;
	   flag = 1;
    };


    if (flag)
    {
        I2Cx_Master_BusError();                
    }
	#endif
    else
    {
        if (I2Cx_Master_HandlerFn != NULL)
            I2Cx_Master_HandlerFn(u32Status);
    }
}

void I2Cx_MasterRx_multi(unsigned long u32Status)
{
    if(u32Status == MASTER_START_TRANSMIT) //0x08                       	/* START has been transmitted and prepare SLA+W */
    {
		clr_STA;      														//STA bit should be cleared by software 
		I2DAT = (g_u8DeviceAddr_m << 1) | I2C_WR;    						//load SLA+W/R 

		set_SI;
    }
    else if(u32Status == MASTER_TRANSMIT_ADDRESS_ACK) //0x18        			/* SLA+W has been transmitted and ACK has been received */
    {
		I2DAT = g_au8Reg;  													//load DATA 

		set_SI;
    }
    else if(u32Status == MASTER_TRANSMIT_ADDRESS_NACK) //0x20            	/* SLA+W has been transmitted and NACK has been received */
    {
		set_STO;      														//transmit STOP 		
		set_SI;
		set_STA;

		set_AA;      														//ready for ACK own SLA+W/R or General Call 
    }
    else if(u32Status == MASTER_TRANSMIT_DATA_ACK) //0x28                  	/* DATA has been transmitted and ACK has been received */
    {
        if (rawlenth > 0)														//repeat start
        {
//			I2C_SET_CONTROL_REG(MASTER_I2C, I2C_CTL_SI | I2C_CTL_STA);	
		
			set_SI;
			set_STA;
        }
			
		else
		{
//			I2C_SET_CONTROL_REG(MASTER_I2C, I2C_CTL_SI | I2C_CTL_STO);
		
			set_SI;
			set_STO;

			g_u8EndFlag = 1;
		}
    }	
    else if(u32Status == MASTER_REPEAT_START) //0x10                  		/* Repeat START has been transmitted and prepare SLA+R */
    {
		clr_STA;      														//STA bit should be cleared by software 
		I2DAT = (g_u8DeviceAddr_m << 1) | I2C_RD;    						//load SLA+W/R 		

		set_SI;
    }
    else if(u32Status == MASTER_RECEIVE_ADDRESS_ACK) //0x40                	/* SLA+R has been transmitted and ACK has been received */
    {	    	
		if (rawlenth > 1)
		{
			set_SI;
			set_AA;      													//ACK next received DATA 

		}
		else
		{
			set_SI;
		}
    }
    else if(u32Status == MASTER_RECEIVE_ADDRESS_NACK) //0x48                	/* SLA+R transmitted, NACK received */
    {	    	
	      set_STO; 
	      set_AA; 
    }	
	else if(u32Status == MASTER_RECEIVE_DATA_ACK) //0x50                 	/* DATA has been received and ACK has been returned */
    {
        g_au8Buffer[g_u8DataLen_m++] = I2DAT;
        if (g_u8DataLen_m < (rawlenth-1))	//if continuing receiving DATA
		{
			set_SI;
			set_AA;
		}
		else	//if last DATA will be received , not ACK next received DATA 
		{
			clr_AA;
		}
	}
    else if(u32Status == MASTER_RECEIVE_DATA_NACK) //0x58                  	/* DATA has been received and NACK has been returned */
    {
		g_au8Buffer[g_u8DataLen_m++] = I2DAT; 
		set_SI;
		set_STO; 
//		set_AA; 
		
        g_u8EndFlag = 1;
    }
    else
    {
		I2Cx_Master_BusError();
    }
}

void I2Cx_MasterTx_multi(unsigned long u32Status)
{
    if(u32Status == MASTER_START_TRANSMIT)  //0x08                     	/* START has been transmitted */
    {
		clr_STA;      													//STA bit should be cleared by software 
		I2DAT = (g_u8DeviceAddr_m << 1) | I2C_WR;    					//load SLA+W/R 

		set_SI;
		
    }
    else if(u32Status == MASTER_TRANSMIT_ADDRESS_ACK)  //0x18           	/* SLA+W has been transmitted and ACK has been received */
    {	
		I2DAT = g_au8Reg;  												//load DATA 
		
		set_SI;
    }
    else if(u32Status == MASTER_TRANSMIT_ADDRESS_NACK) //0x20           /* SLA+W has been transmitted and NACK has been received */
    {
		set_STO;      													//transmit STOP 	
		set_SI;
		set_STA;			

		set_AA;      													//ready for ACK own SLA+W/R or General Call 
    }
    else if(u32Status == MASTER_TRANSMIT_DATA_ACK) //0x28              	/* DATA has been transmitted and ACK has been received */
    {	
        if(g_u8DataLen_m < rawlenth)										//if continuing to send DATA 
        {
			I2DAT = g_au8Buffer[g_u8DataLen_m++];
			set_SI;
        }
        else																//if no DATA to be sent 
        {
			set_STO; 			
			set_SI;
			set_AA; 
            g_u8EndFlag = 1;
        }	
    }
    else if(u32Status == MASTER_TRANSMIT_DATA_NACK) //0x30
    {
		set_STO; 
		set_AA; 
    }	
    else if(u32Status == MASTER_ARBITRATION_LOST) //0x38
    {
	    set_STA;      													//retry to transmit START if bus free 
	    set_SI;
		set_AA; 
    }
    else if(u32Status == BUS_ERROR) 				//0x00
    {
	    set_STO;      													//recover from bus error 
	    set_SI;
		set_AA;

	    set_SI;
		set_AA;		
    }		
    else
    {	
		I2Cx_Master_BusError();
    }
}

void I2Cx_Write_Multi_ToSlave(unsigned char address,unsigned int reg,unsigned char *array,unsigned int len)
{		
	#if defined (MASTER_I2C_USE_POLLING)
	unsigned char i = 0;
    unsigned char address_8bit = address << 1;
	
    I2C_start();
    I2C_write(address_8bit|I2C_WR);
//    I2C_write(HIBYTE(reg));
    I2C_write(LOBYTE(reg));

	#if 1
	for (i = 0 ; i < len ; i++)
	{
    	I2C_write(array[i]);
	}
	#else
	I2C_write(*array);
	#endif

    I2C_stop();

 
	#elif defined (MASTER_I2C_USE_IRQ)
	/* u8SlaveAddr     Access Slave address(7-bit) */
	unsigned char u8SlaveAddr = address ;
	
	g_u8DeviceAddr_m = u8SlaveAddr;
	rawlenth = len;
	g_au8Reg = reg;
	g_au8Buffer = array;

	g_u8DataLen_m = 0;
	g_u8EndFlag = 0;

	/* I2C function to write data to slave */
	I2Cx_Master_HandlerFn = (I2C_FUNC)I2Cx_MasterTx_multi;

//	printf("I2Cx_MasterTx_multi finish\r\n");

	/* I2C as master sends START signal */
	set_STA;

	/* Wait I2C Tx Finish */
	while(g_u8EndFlag == 0);
	g_u8EndFlag = 0;

	#endif
	
}

void I2Cx_Read_Multi_FromSlave(unsigned char address,unsigned int reg,unsigned char *array,unsigned int len)
{ 
	#if defined (MASTER_I2C_USE_POLLING)
    unsigned char i = 0;	
    unsigned char value = 0x00;
    unsigned char address_8bit = address << 1;
	
    I2C_start();
    I2C_write(address_8bit|I2C_WR);
//    I2C_write(HIBYTE(reg));
    I2C_write(LOBYTE(reg));
	
    I2C_start();
    I2C_write(address_8bit|I2C_RD);

	#if 1	
	for (i = 0; i < len; i++) 
	{
		if (i == (len - 1)) 
		{
			//Last byte
			array[i] = I2C_read(I2C_NACK);
		} 
		else 
		{
			array[i] = I2C_read(I2C_ACK);
		}
	}

	
	#else
	*array = I2C_read(I2C_NACK);
	#endif
	
    I2C_stop();

 
	#elif defined (MASTER_I2C_USE_IRQ)
	/* u8SlaveAddr     Access Slave address(7-bit) */
	unsigned char u8SlaveAddr = address ;

	g_u8DeviceAddr_m = u8SlaveAddr;
	rawlenth = len;
	g_au8Reg = reg ;
	g_au8Buffer = array;

	g_u8EndFlag = 0;
	g_u8DataLen_m = 0;

	/* I2C function to read data from slave */
	I2Cx_Master_HandlerFn = (I2C_FUNC)I2Cx_MasterRx_multi;

//	printf("I2Cx_MasterRx_multi finish\r\n");
	
	set_STA;

	/* Wait I2C Rx Finish */
	while(g_u8EndFlag == 0);
	
	#endif	
}

void I2Cx_Master_Init(void)
{    
	P13_OPENDRAIN_MODE;		// SCL
	P14_OPENDRAIN_MODE;		// SDA
	clr_I2CPX;

    I2CLK = I2C_CLOCK;

	#if defined (MASTER_I2C_USE_IRQ)
    set_EI2C;                            //enable I2C interrupt by setting IE1 bit 0
    set_EA;
	#endif

	set_I2TOC_I2TOCEN;
	
    set_I2CON_I2CEN;	
}

//void I2Cx_Master_example (unsigned char res)
//{
//	
//	#if 0
//	UNUSED_VARIABLE(res);
//	#else
//	static unsigned long cnt = 0;	
//	unsigned char u8RxData[16] = {0};
//	unsigned char u8TxData[16] = {0};
//	unsigned char u8CalData[16] = {0};
//	
//	static unsigned int DataCnt = 10;	

//	unsigned char addr,reg;
//	unsigned long i;	
//	unsigned int len;
//	unsigned char crc = 0;
//	
//	printf("I2Cx_Master_example start\r\n");

//	//	I2Cx_Master_Init();

//	//clear data
//	for(i = 0; i < 16; i++)
//	{
//		u8TxData[i] = 0;
//	}

//	switch(res)
//	{
//		case 1 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x66;
//			len = 10;
//		
//			//fill data
//			u8TxData[0] = 0x12 ;
//			u8TxData[1] = 0x34 ;	
//			u8TxData[2] = 0x56 ;
//			u8TxData[3] = 0x78 ;
//			u8TxData[4] = 0x90 ;
//			u8TxData[5] = 0xAB ;
//			u8TxData[6] = 0xCD ;
//			u8TxData[7] = 0xEF ;
//			u8TxData[8] = 0x99 ;
//			u8TxData[9] = 0xFE ;

//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			
//			printf("I2Cx_Write finish\r\n");
//			printf("addr : 0x%2X, reg : 0x%2X , data (%2d) : \r\n",addr,reg,cnt++);
//			
//			break;

//		case 2 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x66;
//			len = 10;
//		
//			//clear data
//			for(i = 0; i < 16; i++)
//			{
//				u8RxData[i] = 0;
//			}

//			I2Cx_Read_Multi_FromSlave(addr,reg,u8RxData,len);
//			
//			printf("\r\nI2Cx_Read  finish\r\n");
//			
//			for(i = 0; i < 16 ; i++)
//			{
//				printf("0x%2X ,", u8RxData[i]);
//		   	 	if ((i+1)%8 ==0)
//		        {
//		            printf("\r\n");
//		        }		
//			}

//			printf("\r\n\r\n\r\n");
//		
//			break;

//		case 3 :
//			addr = I2C_ADDRESS;	//0x15;				
//			reg = 0x00;
//			len = 2;

//			//calculate crc
//			u8CalData[0] = reg ;	//flag
//			u8CalData[1] = DataCnt;	//duty
//			crc = CRC_Get(u8CalData , 2);
//			
//			//fill data
//			u8TxData[0] = u8CalData[1] ;
//			u8TxData[1] = crc;

//			DataCnt = (DataCnt >= 100) ? (0) : (DataCnt+10) ;

//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			
//			printf("I2Cx_Write finish\r\n");
//			printf("addr : 0x%2X, reg : 0x%2X , data (%2d) : \r\n",addr,reg,cnt++);
//			
//			break;

//		case 4 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x01;
//			len = 5;

//			//calculate crc
//			u8CalData[0] = reg ;	//flag
//			u8CalData[1] = 0x00;	//type
//			u8CalData[2] = 10;
//			u8CalData[3] = 20;
//			u8CalData[4] = 30;			
//			crc = CRC_Get(u8CalData , 5);
//			
//			//fill data
//			u8TxData[0] = u8CalData[1];
//			u8TxData[1] = u8CalData[2];
//			u8TxData[2] = u8CalData[3] ;
//			u8TxData[3] = u8CalData[4];
//			u8TxData[4] = crc;

//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			
//			printf("I2Cx_Write finish\r\n");
//			printf("addr : 0x%2X, reg : 0x%2X , data (%2d) : \r\n",addr,reg,cnt++);
//			
//			break;

//		case 5 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x02;
//			len = 6;

//			//calculate crc
//			u8CalData[0] = reg ;	//flag
//			u8CalData[1] = 0x01;//type
//			u8CalData[2] = 0x1E;
//			u8CalData[3] = 0x2E;
//			u8CalData[4] = 0x3E;			
//			crc = CRC_Get(u8CalData , 5);
//			
//			//fill data
//			u8TxData[0] = u8CalData[0];
//			u8TxData[1] = u8CalData[1];
//			u8TxData[2] = u8CalData[2];
//			u8TxData[3] = u8CalData[3];
//			u8TxData[4] = u8CalData[4];
//			u8TxData[5] = crc;
//			
//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			
//			printf("I2Cx_Write finish\r\n");
//			printf("addr : 0x%2X, reg : 0x%2X , data (%2d) : \r\n",addr,reg,cnt++);
//			
//			break;

//		case 6 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x04;
//			len = 4;

//			//calculate crc
//			u8CalData[0] = reg ;	//flag
//			u8CalData[1] = 0x00;	//read page
//			u8CalData[2] = 0x00;	//type			
//			crc = CRC_Get(u8CalData , 3);
//			
//			//fill data
//			u8TxData[0] = u8CalData[0];
//			u8TxData[1] = u8CalData[1];
//			u8TxData[2] = u8CalData[2];
//			u8TxData[3] = crc;	
//			
//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			I2Cx_Read_Multi_FromSlave(addr,reg,u8RxData,9);//16
//				
//			printf("\r\nI2Cx_Read  finish\r\n");
//			
//			for(i = 0; i < 16 ; i++)
//			{
//				printf("0x%2X ,", u8RxData[i]);
//		   	 	if ((i+1)%8 ==0)
//		        {
//		            printf("\r\n");
//		        }		
//			}

//			printf("\r\n\r\n\r\n");			

//			
//			break;

//		case 7 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x04;
//			len = 4;
//			
//			I2Cx_Read_Multi_FromSlave(addr,reg,u8RxData,16);
//		
//			printf("\r\nI2Cx_Read  finish\r\n");
//			
//			for(i = 0; i < 16 ; i++)
//			{
//				printf("0x%2X ,", u8RxData[i]);
//		   	 	if ((i+1)%8 ==0)
//		        {
//		            printf("\r\n");
//		        }		
//			}

//			printf("\r\n\r\n\r\n");			
//			
//			break;
//			
//		case 9 :
//			addr = I2C_ADDRESS;	//0x15;
//			reg = 0x66;
//			len = 10;
//		
//			//fill data
//			u8TxData[0] = 0x12 ;
//			u8TxData[1] = 0x34 ;	
//			u8TxData[2] = 0x56 ;
//			u8TxData[3] = 0x78 ;
//			u8TxData[4] = 0x90 ;
//			u8TxData[5] = 0xAB ;
//			u8TxData[6] = 0xCD ;
//			u8TxData[7] = 0xEF ;
//			u8TxData[8] = 0x99 ;
//			u8TxData[9] = 0xFE ;

//			I2Cx_Write_Multi_ToSlave(addr,reg,u8TxData,len);
//			
//			printf("I2Cx_Write finish\r\n");
//			printf("addr : 0x%2X, reg : 0x%2X , data (%2d) : \r\n",addr,reg,cnt++);
//			
//			//clear data
//			for(i = 0; i < 16; i++)
//			{
//				u8RxData[i] = 0;
//			}

//			I2Cx_Read_Multi_FromSlave(addr,reg,u8RxData,len);
//			
//			printf("\r\nI2Cx_Read  finish\r\n");
//			
//			for(i = 0; i < 16 ; i++)
//			{
//				printf("0x%2X ,", u8RxData[i]);
//		   	 	if ((i+1)%8 ==0)
//		        {
//		            printf("\r\n");
//		        }		
//			}

//			printf("\r\n\r\n\r\n");

//			
//			break;
//	}	
//	#endif
//}



