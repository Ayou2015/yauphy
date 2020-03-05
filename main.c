#include "cbm8007.h"
#include "uart.h"
#include "flashprg.h"

#include "Image.h"
#include ".\Algorithm\FingerPrint_API.h"

#include <core_cmInstr.h>

u32 uTemp;
u16 wDelay;
u32 dwBuf[42];
int result;

MFEATURE WaitFeature; //模板地址
MFEATURE TempFeature;

void Uart_Test()
{
	uart_init(115200, 1, PARI_NONE);

	uart_puts("cbm8007 uart test");	//?????
	uart_get((u8*)dwBuf, 100);	//?????


	REG32(ADDR_UART_IE)  |= (0x01 << 6);	/* UART RX IE Enable */
//	REG32(ADDR_UART_IE)  |= (0x01 << 7);	/* UART TX IE Enable */
	NVIC_EnableIRQ(IRQ_UART); //??????;
	
}

void __irq intr_uart(void)
{
	u8 c;
	u32 sta = REG32(ADDR_USTAT);
	struct uart_rx_buf *rx = &uart_rx_buf;


	if (sta & (1<<3))	//rx int
	{
		c = REG_ACCESS_BYTE(ADDR_URXD, 0);
		uart_write_char(c);
	}
	if (sta & (1<<4))	//tx int
	{
		if(rx->cnt != 0)
		{
			uart_putc(uart_read_char());
			REG32(ADDR_UART_IE)  |= (1 << 7);	/* UART TX IE Enable */
		}
		else
			REG32(ADDR_UART_IE) &= ~(1 << 7);	/* UART TX IE disable */
	}
	REG32(ADDR_USTAT) &= (~0x07);
	result = 0;
}

void delay(u32 cnt)
{
	while(cnt--);
}


int main(void)
{

	int i,j;
	int s;
	

	uTemp = SCB->CPUID;	//for test
	
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;	//退出中断后重新sleep

	
	for(i=0;i<1;i++)
  {
		
	 uart_init(115200, 1, PARI_NONE);
		uart_puts("cbm8007: \r\n");	//?????
		
		//Uart_Test();
		delay(10000);
	}
	
	
	ExtractTemplate(FP_Data1, &TempFeature);
	uart_print("TempFeature has been extracted:%d, %d\r\n", TempFeature.pMinutiaeSets.nMinutiaeNum, TempFeature.pMinutiaeSets.nKneePointNum);
	
	ExtractTemplate(FP_Data, &WaitFeature);
	uart_print("WaitFeature has been extracted:%d, %d\r\n", WaitFeature.pMinutiaeSets.nMinutiaeNum, WaitFeature.pMinutiaeSets.nKneePointNum);
	
	

	s = MatchTemplate(&TempFeature,&WaitFeature, 0);
	uart_print("matchScore:%d",s);
	

	REG32(ADDR_SYS_CFG)|=(1<<8);	//bit8置1断电

	while(1);	//不应该跑到这里来
}




