#ifndef DEV_UART1_H
#define DEV_UART1_H

#include "conf.h"

#define DEV_UART1_RX_BUF_MAX        32
#define DEV_UART1_TX_BUF_MAX        32
#define DEV_UART1_IDLE_TICK_MAX     2       

#define DEV_UART1_BAUD_RATE         115200UL

int     dev_uart1_init          (void);
void    dev_uart1_put_char      (char c);
void    dev_uart1_write         (const char* buf, uint16_t buf_sz);
int     dev_uart1_read          (char* buf, uint16_t buf_sz);

void    dev_uart1_rx_idle_check     (tick_t curr);
void    dev_uart1_rx_irq_handler    (void);
void    dev_uart1_tx_irq_handler    (void);



#endif	// DEV_UART1_H