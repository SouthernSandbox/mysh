#include "dev_uart.h"
#include "dev_timer0.h"

struct uart1_mgr
{
    struct uart1_mgr_rd
    {
        char buf[DEV_UART1_RX_BUF_MAX];      /** 接收缓冲区 **/
        uint16_t w_ptr;       /** 写入指针 **/
        uint16_t ptr;        /** 读取指针 **/
    } rd;
    
    tick_t record_tick;     /** 记录当前的 tick，用于判定是否产生空闲中断 **/

    bool_t tx_busy          : 1;        /** 发送忙状态 **/
    bool_t is_rx_finish     : 1;        /** 接收完成标志位 **/
};
typedef struct uart1_mgr* uart1_mgr_t;
static struct uart1_mgr mgr;


static void _uart1_gpio_init(void)
{
    /**
     * @brief UART1 GPIO
     *          P3.0    RXD
     *          P3.1    TXD
     * 
     * @note 引脚配置为输入弱上拉模式
     */

    /** 将引脚设置为上拉，准双向口 **/
    P3M1 &= ~(0x01 | 0x02); 
	P3M0 &= ~(0x01 | 0x02);
}

static void _uart1_dev_init(void)
{
     /**
     * @brief UART1 
     *          使用定时器  timer1
     *          定时器为 1T 模式
     *          使用波特率  115200
     *          不使用双倍波特率
     */

    /** C51 变量声明 **/
    uint16_t timer_val;

    /** GPIO 初始化 **/
    _uart1_gpio_init();

    /** 清空缓冲区 **/
    memset(&mgr, 0, sizeof(struct uart1_mgr));

    /** 计算定时器时长 **/
    timer_val = (MCU_SYS_FOSC / 4) / DEV_UART1_BAUD_RATE;
    timer_val = 65536UL - timer_val;

    /** 设置串口和定时器 **/
    SCON        = (SCON & 0x3f) | (1 << 6);	    // 模式设置
    TR1         = 0;
    AUXR        &= ~0x1;		                // 将 UART1 的波特率发生器设置为 TIMER1
    TMOD        &= ~(1 << 6);	                // 将 TIMER1 设置为定时器模式
    TMOD        &= ~0x30;		                // 将 TIMER1 设置为自动重装载;
    AUXR        |=  (1 << 6);	                // 将 TIMER1 设置为 1T 模式
    TH1         = (timer_val >> 8) & 0xff;
    TL1         = timer_val & 0xff;
    ET1         = 0;	                        // 禁止中断
    TMOD        &= ~0x40;	                    // 定时
    INTCLKO     &= ~0x2;	                    // 不输出时钟
    TR1         = 1;                            // 启动 TIMER1
    REN         = 1;                            // 串口接收使能

    /** 使能串口中断 **/
    ES = 1;

    /** 设置串口中断优先级 **/
    IPH &= ~0x10;
    PS = 1;

    /** 切换功能引脚为 P30,P31 **/
    P_SW1 = (P_SW1 & 0x3F) | (0 << 6);
}


static bool_t _uart1_is_rx_finish (void)
{
    if(mgr.is_rx_finish == true)
    {
        mgr.is_rx_finish = false;
        return true;
    }
    else
        return false;
}











int dev_uart1_init(void)
{
    _uart1_gpio_init();
    _uart1_dev_init();

    return 0;
}


void dev_uart1_put_char(char c)
{
    SBUF = c;
	mgr.tx_busy = true;		//标志忙
	while(mgr.tx_busy);
}


void dev_uart1_write (const char* buf, uint16_t buf_sz)
{
    uint16_t i = 0;
    for (i = 0; i < buf_sz;	i++)  
        dev_uart1_put_char(buf[i]);
}




int dev_uart1_read (char* buf, uint16_t buf_sz)
{
    uint16_t rd_len;
    uint16_t ptr_gap = mgr.rd.w_ptr - mgr.rd.ptr;

    /** 检查串口是否正在接收数据 **/
    if(_uart1_is_rx_finish() == false)  
        return 0;

    /** 校正数据填充到 buf 的数量，避免越界 **/
    rd_len = (ptr_gap > buf_sz) ? (buf_sz) : (ptr_gap);

    /** 复制数据到 buf **/
    memcpy(buf, mgr.rd.buf + mgr.rd.ptr, rd_len);

    /** 读取指针偏移 **/
    mgr.rd.ptr += rd_len;

    /** 检查读取指针和写入指针位置是否重叠，若重叠则认为当前数据已读取完毕 **/
    if(mgr.rd.ptr >= mgr.rd.w_ptr)
        memset(&mgr.rd, 0, sizeof(mgr.rd));

    return rd_len;
}












/**
 * @brief 检查串口接收引脚是否已经空闲，该函数将会被 timer0 的中断回调函数所调用
 * 
 * @param curr 
 * @return int 
 */
void dev_uart1_rx_idle_check (tick_t curr)
{
    /** 若当前已经空闲，则不再向下检查 **/
    if(mgr.is_rx_finish == true)
        return;

    /** 若当前处于串口的状态为未接收结束，则进行空闲判定 **/
    if(curr - mgr.record_tick > DEV_UART1_IDLE_TICK_MAX)
        mgr.is_rx_finish = true;
}



void dev_uart1_rx_irq_handler(void)
{
    /** 读取字符到缓冲区 **/
    mgr.rd.buf[mgr.rd.w_ptr++] = SBUF;

    /** 读取指针重置 **/
    if(mgr.rd.w_ptr >= DEV_UART1_RX_BUF_MAX)
        mgr.rd.w_ptr = 0;

    /** 重置串口接收状态 **/
    mgr.is_rx_finish = false;
    mgr.record_tick = dev_timer0_get_tick();
}


void dev_uart1_tx_irq_handler(void)
{
    /** 重置写忙 **/
    mgr.tx_busy = false;
}
