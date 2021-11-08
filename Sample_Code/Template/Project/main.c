/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "MS51_16K.h"
#include "i2c_conf.h"
#include	"project_config.h"

#include "Main_User.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/


volatile uint8_t u8TH0_Tmp = 0;
volatile uint8_t u8TL0_Tmp = 0;

//UART 0
bit BIT_TMP;
bit BIT_UART;


/*_____ D E F I N I T I O N S ______________________________________________*/
volatile uint32_t BitFlag = 0;
volatile uint32_t counter_tick = 0;

/*_____ M A C R O S ________________________________________________________*/



/*_____ F U N C T I O N S __________________________________________________*/

extern void I2Cx_Master_Init(void);


char putchar (char c)  {
  while (!TI);
  TI = 0;
  return (SBUF = c);
}

void tick_counter(void)
{
	counter_tick++;
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

#if defined (REDUCE_CODE_SIZE)
void compare_buffer(uint8_t *src, uint8_t *des, int nBytes)
{
    uint16_t i = 0;	
	
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			set_flag(flag_error , Enable);
        }
    }

	if (!is_flag_set(flag_error))
	{
    	printf("compare_buffer finish \r\n");	
		set_flag(flag_error , Disable);
	}

}

void reset_buffer(void *dest, unsigned int val, unsigned int size)
{
    uint8_t *pu8Dest;
//    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;

	#if 1
	while (size-- > 0)
		*pu8Dest++ = val;
	#else
	memset(pu8Dest, val, size * (sizeof(pu8Dest[0]) ));
	#endif
	
}

void copy_buffer(void *dest, void *src, unsigned int size)
{
    uint8_t *pu8Src, *pu8Dest;
    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;
    pu8Src  = (uint8_t *)src;


	#if 0
	  while (size--)
	    *pu8Dest++ = *pu8Src++;
	#else
    for (i = 0; i < size; i++)
        pu8Dest[i] = pu8Src[i];
	#endif
}

void dump_buffer(uint8_t *pucBuff, int nBytes)
{
    uint16_t i = 0;
    
    printf("dump_buffer : %2d\r\n" , nBytes);    
    for (i = 0 ; i < nBytes ; i++)
    {
        printf("0x%2X," , pucBuff[i]);
        if ((i+1)%8 ==0)
        {
            printf("\r\n");
        }            
    }
    printf("\r\n\r\n");
}

void  dump_buffer_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%02X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%2BX ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}



void send_UARTString(uint8_t* Data)
{
	#if 1
	uint16_t i = 0;

	while (Data[i] != '\0')
	{
		#if 1
		SBUF = Data[i++];
		#else
		UART_Send_Data(UART0,Data[i++]);		
		#endif
	}

	#endif

	#if 0
	uint16_t i = 0;
	
	for(i = 0;i< (strlen(Data)) ;i++ )
	{
		UART_Send_Data(UART0,Data[i]);
	}
	#endif

	#if 0
    while(*Data)  
    {  
        UART_Send_Data(UART0, (unsigned char) *Data++);  
    } 
	#endif
}

void send_UARTASCII(uint16_t Temp)
{
    uint8_t print_buf[16];
    uint16_t i = 15, j;

    *(print_buf + i) = '\0';
    j = (uint16_t)Temp >> 31;
    if(j)
        (uint16_t) Temp = ~(uint16_t)Temp + 1;
    do
    {
        i--;
        *(print_buf + i) = '0' + (uint16_t)Temp % 10;
        (uint16_t)Temp = (uint16_t)Temp / 10;
    }
    while((uint16_t)Temp != 0);
    if(j)
    {
        i--;
        *(print_buf + i) = '-';
    }
    send_UARTString(print_buf + i);
}

void send_UARTHex(uint16_t u16Temp)
{
    uint8_t print_buf[16];
    uint32_t i = 15;
    uint32_t temp;

    *(print_buf + i) = '\0';
    do
    {
        i--;
        temp = u16Temp % 16;
        if(temp < 10)
            *(print_buf + i) = '0' + temp;
        else
            *(print_buf + i) = 'a' + (temp - 10) ;
        u16Temp = u16Temp / 16;
    }
    while(u16Temp != 0);
    send_UARTString(print_buf + i);
}
#endif

void GPIO_Init(void)	//for test
{
    P12_PUSHPULL_MODE;	

}

void Timer_Delay(unsigned long u32SYSCLK, unsigned int u16CNT, unsigned int u16DLYUnit)
{
    unsigned char TL1TMP,TH1TMP;

    TIMER1_FSYS_DIV12;                                 //T1M=0, Timer1 Clock = Fsys/12
    ENABLE_TIMER1_MODE1;                                //Timer1 is 16-bit mode
    TL1TMP = LOBYTE(65535-((u32SYSCLK/1000000)*u16DLYUnit)/12);
    TH1TMP = HIBYTE(65535-((u32SYSCLK/1000000)*u16DLYUnit)/12);
  
    while (u16CNT != 0)
    {
      TL1 = TL1TMP;
      TH1 = TH1TMP;
      set_TCON_TR1;                                //Start Timer1
      while (!TF1);                                //Check Timer1 Time-Out Flag
      clr_TCON_TF1;
      clr_TCON_TR1;
      u16CNT --;
    }
}

void Timer0_IRQHandler(void)
{
//	static uint16_t counter = 0;

	tick_counter();

	if ((get_tick() % 1000) == 0)
	{

		P12 ^= 1;
		
//    	printf("Timer0_IRQHandler : %bd\r\n",counter++);

	}

	if ((get_tick() % 50) == 0)
	{

	}

	
}

void Timer0_ISR(void) interrupt 1        // Vector @  0x0B
{
    _push_(SFRS);	
	
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
    clr_TCON_TF0;
	
	Timer0_IRQHandler();

    _pop_(SFRS);	
}

void Timer0_Init(void)
{
	uint16_t res = 0;

	ENABLE_TIMER0_MODE1;

	#if defined (ENABLE_16MHz)
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_160000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_160000); 

	
	#elif defined (ENABLE_24MHz)
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000); 
	#endif
	
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;

    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                  //Timer0 run
}

void UART0_Process(uint8_t res)
{
	if (res > 0x7F)
	{
		return;
	}

	if (is_flag_set(flag_uart0_receive))
	{
		set_flag(flag_uart0_receive , Disable);

		#if 0

		res -= 0x30;
		I2Cx_Master_example(res);
		
		#else
//		printf("0x%2X\r\n" , res);
		switch(res)
		{
			case '1' :

				break;

			case '2': 
			
				break;

			case '3': 
			
				break;			

			case '4': 
		
				break;	

			case '5': 
		
				break;	

			case '6': 
		
				break;	

			case '0' : 

				break;
		}	
		#endif
	}
}

void Serial_ISR (void) interrupt 4 
{
    if (RI)
    {   
	  set_flag(flag_uart0_receive , Enable);
	  UART0_Process(SBUF);

      clr_SCON_RI;                                         // Clear RI (Receive Interrupt).
    }
    if  (TI)
    {
      if(!BIT_UART)
      {
          TI = 0;
      }
    }
}

void UART0_Init(void)
{
	unsigned long u32Baudrate = 115200;
	
	P06_QUASI_MODE;    //Setting UART pin as Quasi mode for transmit
	
	#if 1
	SCON = 0x50;          //UART0 Mode1,REN=1,TI=1
	set_PCON_SMOD;        //UART0 Double Rate Enable
	T3CON &= 0xF8;        //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
	set_T3CON_BRCK;        //UART0 baud rate clock source = Timer3

	#if defined (ENABLE_16MHz)
	RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  
	RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);  
	#elif defined (ENABLE_24MHz)
	RH3    = HIBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	RL3    = LOBYTE(65536 - (SYS_CLOCK/16/u32Baudrate));  
	#endif
	
	set_T3CON_TR3;         //Trigger Timer3
	set_IE_ES;

	ENABLE_GLOBAL_INTERRUPT;

	set_SCON_TI;
	BIT_UART=1;
	#else	
	
	SCON = 0x50;       			//UART0 Mode1,REN=1,TI=1
	TMOD |= 0x20;      		//Timer1 Mode1
	set_PCON_SMOD;          	//UART0 Double Rate Enable
	set_CKCON_T1M;
	clr_T3CON_BRCK;          	//Serial port 0 baud rate clock source = Timer1
	TH1 = 256 - (SYS_CLOCK/16/u32Baudrate);

	set_TCON_TR1;
	ENABLE_UART0_INTERRUPT;

	ENABLE_GLOBAL_INTERRUPT;
	
	set_SCON_TI;
	BIT_UART=1;
	
	#endif
}



#if defined (ENABLE_16MHz)
void MODIFY_HIRC_16(void)
{
    unsigned char data hircmap0,hircmap1;
    set_CHPCON_IAPEN;
    IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    hircmap0 = IAPFD;
    IAPAL = 0x31;
    IAPAH = 0x00;
    set_IAPTRG_IAPGO;
    hircmap1 = IAPFD;
    clr_CHPCON_IAPEN;
    TA=0XAA;
    TA=0X55;
    RCTRIM0 = hircmap0;
    TA=0XAA;
    TA=0X55;
    RCTRIM1 = hircmap1;
}

#elif defined (ENABLE_24MHz)
void MODIFY_HIRC_24(void)
{
    unsigned char data hircmap0,hircmap1;
/* Check if power on reset, modify HIRC */
    if (PCON&SET_BIT4)
    {
        set_CHPCON_IAPEN;
        IAPAL = 0x38;
        IAPAH = 0x00;
        IAPCN = READ_UID;
        set_IAPTRG_IAPGO;
        hircmap0 = IAPFD;
        IAPAL = 0x39;
        IAPAH = 0x00;
        set_IAPTRG_IAPGO;
        hircmap1 = IAPFD;
        clr_CHPCON_IAPEN;
        TA=0XAA;
        TA=0X55;
        RCTRIM0 = hircmap0;
        TA=0XAA;
        TA=0X55;
        RCTRIM1 = hircmap1;
        clr_CHPCON_IAPEN;
    }
}

#endif

void SYS_Init(void)
{
	#if defined (ENABLE_16MHz)
	MODIFY_HIRC_16();
	#else
    MODIFY_HIRC_24();
	#endif

    ALL_GPIO_QUASI_MODE;
    ENABLE_GLOBAL_INTERRUPT;                // global enable bit	
}

void main (void) 
{
//	uint8_t res = 1;
	
    SYS_Init();

    UART0_Init();
							
	GPIO_Init();					

	I2Cx_Master_Init();
  	INIT_VCNL36687();
  
	Timer0_Init();

	
    while(1)
    {
		PRINT_VCNL36687();

    }
}



