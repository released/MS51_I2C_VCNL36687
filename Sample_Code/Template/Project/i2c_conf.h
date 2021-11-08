


//#define ENABLE_16MHz
#define ENABLE_24MHz

#if defined (ENABLE_16MHz)
#define I2C_CLOCK               					(0x09) // default : 400k , formula : FSYS / (4* (I2CLK+1) )

#elif defined (ENABLE_24MHz)
//#define I2C_CLOCK               					(0x0E) // default : 400k , formula : FSYS / (4* (I2CLK+1) )
#define I2C_CLOCK               					(0x3B) // 100k , formula : FSYS / (4* (I2CLK+1) )
#endif

#if defined (ENABLE_16MHz)
#define SYS_CLOCK 								(16000000ul)
#elif defined (ENABLE_24MHz)
#define SYS_CLOCK 								(24000000ul)
#endif


#ifndef NULL
#define NULL      (0)                 			///< NULL pointer
#endif

#define UNUSED_VARIABLE(x) 						((void)(x))

//#define I2C_ADDRESS               				(0xA0)	//(0x15>>1)

//#define MASTER_I2C						  		(I2C0)
//#define MASTER_I2C_IRQn						  	(I2C0_IRQn)
//#define I2Cx_Master_IRQHandler					(I2C0_IRQHandler)

//#define MASTER_I2C						  		(I2C1)
//#define MASTER_I2C_IRQn						  	(I2C1_IRQn)
//#define I2Cx_Master_IRQHandler					(I2C1_IRQHandler)


#define I2C_timeout_count                 		(1000)
#define I2C_WR               					(0x00)
#define I2C_RD               					(0x01)
#define I2C_ACK 									(0)
#define I2C_NACK 								(1)


#define MASTER_START_TRANSMIT			  		(0x08)
#define MASTER_REPEAT_START               		(0x10)
#define MASTER_TRANSMIT_ADDRESS_ACK       	(0x18)
#define MASTER_TRANSMIT_ADDRESS_NACK      	(0x20)
#define MASTER_TRANSMIT_DATA_ACK          	(0x28)
#define MASTER_TRANSMIT_DATA_NACK         	(0x30)
#define MASTER_ARBITRATION_LOST           		(0x38)
#define MASTER_RECEIVE_ADDRESS_ACK        		(0x40)
#define MASTER_RECEIVE_ADDRESS_NACK       		(0x48)
#define MASTER_RECEIVE_DATA_ACK           		(0x50)
#define MASTER_RECEIVE_DATA_NACK          		(0x58)
#define BUS_ERROR                         		(0x00)

//#define DEBUG_LOG_MASTER_LV1

//#define MASTER_I2C_USE_IRQ
#define MASTER_I2C_USE_POLLING


//#define SLAVE_I2C						  		(I2C0)
//#define SLAVE_I2C_IRQn						  	(I2C0_IRQn)
//#define I2Cx_Slave_IRQHandler						(I2C0_IRQHandler)

#define SLAVE_TRANSMIT_REPEAT_START_OR_STOP	(0xA0)
#define SLAVE_TRANSMIT_ADDRESS_ACK 			(0xA8)
#define SLAVE_TRANSMIT_DATA_ACK				(0xB8)
#define SLAVE_TRANSMIT_DATA_NACK             	(0xC0)
#define SLAVE_TRANSMIT_LAST_DATA_ACK         	(0xC8)
#define SLAVE_RECEIVE_ADDRESS_ACK            	(0x60)
#define SLAVE_RECEIVE_ARBITRATION_LOST       	(0x68)
#define SLAVE_RECEIVE_DATA_ACK               	(0x80)
#define SLAVE_RECEIVE_DATA_NACK              	(0x88)
#define GC_MODE_ADDRESS_ACK                	(0x70)
#define GC_MODE_ARBITRATION_LOST             	(0x78)
#define GC_MODE_DATA_ACK                   	(0x90)
#define GC_MODE_DATA_NACK                  	(0x98)
#define ADDRESS_TRANSMIT_ARBITRATION_LOST    	(0xB0)

//#define DEBUG_LOG_SLAVE_LV1
#define DEBUG_LOG_SLAVE_LV2


void I2Cx_Master_BusError(void);
void I2Cx_Write_Multi_ToSlave(unsigned char address,unsigned int reg,unsigned char *array,unsigned int len);
void I2Cx_Read_Multi_FromSlave(unsigned char address,unsigned int reg,unsigned char *array,unsigned int len);
void I2Cx_Master_Init(void);

void I2C_start(void);
void I2C_stop(void);
unsigned char I2C_read(unsigned char ack_mode);
void I2C_write(unsigned char value);

void Timer2_Delay(unsigned long u32SYSCLK,unsigned int u16TMDIV, unsigned int u16CNT, unsigned long u32DLYUnit);
void Timer_Delay(unsigned long u32SYSCLK, unsigned int u16CNT, unsigned int u16DLYUnit);


