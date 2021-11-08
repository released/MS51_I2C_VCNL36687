


#define VCNL36687_SlaveAddress    		0x60   ///< I2C address of the sensor

/** Registers */
#define VCNL36687_PS_CONF1      		0x03
#define VCNL36687_PS_CONF2      		0x03
#define VCNL36687_PS_CONF3      		0x04
#define VCNL36687_PS_CONF4      		0x04

#define VCNL36687_PS_THDL     			0x05
#define VCNL36687_PS_THDH     			0x06
#define VCNL36687_PS_CANC     			0x07
#define VCNL36687_PS_CONF5      		0x08
#define VCNL36687_PS_DATA     			0xF2
#define VCNL36687_PS_INT_FLAG   			0xF3
#define VCNL36687_PS_ID       			0xF4

//CONF1 Datasheet Register 0x03 Low Byte
#define VCNL36687_PS_PERIOD_8ms   		0x00<<6
#define VCNL36687_PS_PERIOD_16ms  		0x01<<6
#define VCNL36687_PS_PERIOD_32ms  		0x02<<6
#define VCNL36687_PS_PERIOD_64ms  		0x03<<6
#define VCNL36686_PS_PERS_1     		0x00<<4
#define VCNL36687_PS_PERS_2     		0x01<<4
#define VCNL36687_PS_PERS_3     		0x02<<4
#define VCNL36687_PS_PERS_4     		0x03<<4
#define VCNL36687_PS_INT_DIS    			0x01<<2
#define VCNL36687_PS_INT_EN     		0x02<<2
#define VCNL36687_PS_INT_Logic    		0x03<<2
#define VCNL36687_PS_SMART_PERS_EN  	0x01<<1
#define VCNL36687_PS_ShutDown   		0x01<<0
#define VCNL36687_PS_ON       			0x00<<0

//CONF2 Datasheet Register 0x03 High Byte
#define VCNL36687_PS_1T       			0x00<<6
#define VCNL36687_PS_2T       			0x01<<6
#define VCNL36687_PS_4T       			0x02<<6
#define VCNL36687_PS_8T       			0x03<<6
#define VCNL36687_PS_MPS_1      		0x00<<4
#define VCNL36687_PS_MPS_2      		0x01<<4
#define VCNL36687_PS_MPS_4      		0x02<<4
#define VCNL36687_PS_MPS_8      		0x03<<4
#define VCNL36687_PS_ITB_25     			0x00<<3
#define VCNL36687_PS_ITB_50     			0x01<<3

//CONF3 Datasheet Register 4 Low Byte
#define VCNL36687_PS_AF_EN      		0x01<<6
#define VCNL36687_PS_Auto     			0x00<<6
#define VCNL36687_PS_FOR_Trig   			0x01<<5

//CONF4 Datasheet Register 4 High Byte
#define VCNL36687_PS_SC_EN      		0x01<<7
#define VCNL36687_PS_SC_CURRx1    		0x00<<5
#define VCNL36687_PS_SC_CURRx2    		0x01<<5
#define VCNL36687_PS_SC_CURRx4    		0x02<<5
#define VCNL36687_PS_SC_CURRx8    		0x03<<5
#define VCNL36687_PS_HD_EN      		0x01<<4
#define VNCL36687_VCSEL_I_7mA   		0x00<<0
#define VCNL36687_VCSEL_I_11mA    		0x01<<0
#define VCNL36687_VCSEL_I_14mA    		0x02<<0
#define VCNL36687_VCSEL_I_17mA    		0x03<<0
#define VCNL36687_VCSEL_I_20mA    		0x04<<0

//CONF5 Datasheet Register 9 Low Byte
#define VCNL36887_POR_S       			0x01<<0

void INIT_VCNL36687(void);
void PRINT_VCNL36687(void);


