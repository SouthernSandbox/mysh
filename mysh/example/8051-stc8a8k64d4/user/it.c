#include "conf.h"
#include "dev_timer0.h"
#include "dev_uart.h"


void TIME0_ISR_Handler (void) interrupt TMR0_VECTOR
{
    EA = 0;

    /** timer0 中断回调函数 **/
    dev_timer0_irq_handler();

    /** UART1 接收空闲检查函数 **/
    dev_uart1_rx_idle_check(dev_timer0_get_tick());

    EA = 1;
}


void UART1_ISR_Handler (void) interrupt UART1_VECTOR
{
    EA = 0;
    
	if(RI)  // 接收中断
    {
        RI = 0;
        dev_uart1_rx_irq_handler();
    }

	if(TI)  // 发送中断
	{
        TI = 0;
        dev_uart1_tx_irq_handler();
	}
    
    EA = 1;
}