#include "dev_timer0.h"

#define TIMER0_TL       0x66
#define TIMER0_TH       0x7E

struct sys_tick_mgr
{
	tick_t tick;
};
static struct sys_tick_mgr mgr;



void dev_timer0_init (void)
{
    /** 定时为 1ms **/
	AUXR |= 0x80;		// 定时器时钟1T模式
	TMOD &= 0xF0;		// 设置定时器模式
	TL0 = TIMER0_TL;	// 设置定时初始值
	TH0 = TIMER0_TH;	// 设置定时初始值
	TF0 = 0;		    // 清除TF0标志
	TR0 = 1;		    // 定时器0开始计时
    ET0 = 1;            // 启用定时器0中断

    dev_timer0_start();

    /** 初始化 mgr **/
    mgr.tick = 0;
}


tick_t dev_timer0_get_tick (void)
{
    return mgr.tick;
}


void dev_timer0_blocked_mdelay (uint32_t ms)
{
	uint32_t record = mgr.tick;
	while(1)
	{
		if(mgr.tick - record >= ms)
			break;
	}
}








void dev_timer0_irq_handler(void)
{
    ++mgr.tick;
}