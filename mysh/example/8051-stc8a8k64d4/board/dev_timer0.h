#ifndef DEV_TIMER0_H
#define DEV_TIMER0_H

#include "conf.h"

#define dev_timer0_start()      TR0 = 1
#define dev_timer0_stop()       TR0 = 0

void dev_timer0_init (void);

void    dev_timer0_blocked_mdelay   (uint32_t ms);
tick_t  dev_timer0_get_tick         (void);
void    dev_timer0_irq_handler      (void);

#endif	// DEV_TIMER0_H