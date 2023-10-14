/**
 * @file    mysh_conf.c
 * @author  Southern Sandbox
 * @brief   mysh 的配置与类型定义文件
 * @version 0.1
 * @date    2023-10-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MYSH_CONF_H
#define MYSH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


/** 基本配置 **/
/* 是否输出彩色字体 */
#define MYSH_SUPPORT_PRINT_MULTI_COLOR          1

/* 初始化时，将打印输出设置为不可见 */
#define MYSH_SUPPORT_OUTPUT_UNVISIBLE_IN_INIT   0

/* 是否支持 Tab 键指令补全 */
#define MYSH_SUPPORT_TAB_COMPETION              1

/* 是否添加内置指令 */
#define MYSH_SUPPORT_INNER_COMMANDS             1

#if MYSH_SUPPORT_INNER_COMMANDS
    #define MYSH_SUPPORT_INNER_COMMANDS_HELP        1       /** help 指令 **/
    #define MYSH_SUPPORT_INNER_COMMANDS_VER         1       /** ver 指令 **/
    #define MYSH_SUPPORT_INNER_COMMANDS_VIS         1       /** vis 指令 **/
    #define MYSH_SUPPORT_INNER_COMMANDS_CLEAR       1       /** clear 指令 **/
#endif

/* 是否支持方向键功能 */
#define MYSH_SUPPORT_DIR_KEY                    1
#define MYSH_SUPPORT_DIR_KEY_FILTER             0       /** 是否支持方向键过滤。启用时会接收方向键键值，但不执行任何操作 **/
                                                        /** 关闭方向键过滤后，将支持光标的左右移动、光标位置字符插入与移除 **/

#if !MYSH_SUPPORT_DIR_KEY
    #warning "After disable the directional key function, any input of directional key values may result in unexpected results"
#endif

/* 是否支持打印时间信息 */
#define MYSH_SUPPORT_PRINT_TIME_INFO            1





/** typedef **/
typedef unsigned char   mysh_uint8_t;
typedef unsigned short  mysh_uint16_t;
typedef unsigned long   mysh_uint32_t;
typedef signed char     mysh_int8_t;
typedef signed short    mysh_int16_t;
typedef signed long     mysh_int32_t;

/* mysh flag 类型 */
typedef mysh_uint8_t            mysh_flag_val_t;

/* mysh opt 类型 */
typedef mysh_uint8_t            mysh_opt_val_t;

/* mysh_cmd_exec 函数指针类型 */
typedef void (*mysh_cmd_exec)(int argc, char* argv[]);





/** 设置接收缓存区的大小 **/
#define MYSH_RX_BUF_MAX         16
#if MYSH_RX_BUF_MAX < 0xff
    typedef mysh_uint8_t        mysh_rd_cnt_t;
#else
    typedef mysh_uint16_t       mysh_rd_cnt_t;
#endif

/** 设置单个指令允许的最多参数 **/
#define MYSH_ARGC_MAX           3

/** 设置指令数量 **/
#define MYSH_CMD_COUNT_MAX      6
#if MYSH_CMD_COUNT_MAX < 0xff
    typedef mysh_uint8_t        mysh_cmd_cnt_t;
#else
    typedef mysh_uint16_t       mysh_cmd_cnt_t;
#endif







/** 用户输入字符定义 **/
#define MYSH_DELIM_CHAR     ' '     /** 分割符 **/



/** 关键字 **/
#define __SAVE_IN_FLASH             const
#define MYSH_NBR_TO_STR(nbr)        #nbr



#if MYSH_SUPPORT_DIR_KEY
    /** 方向键键值定义 **/
    #define MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD_1          '\033'      /** 基于 VT100 控制序列 **/
    #if !defined(__C51__)
        #define MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD_2      '\xe0'      /** 基于键盘码值 **/
    #endif

    #define MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD            MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD_2

    #if (MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD == MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD_1)
        #define MYSH_CTRL_SEQUENCE_DIR_UP_KEY           '\033' + '[' + 'A'
        #define MYSH_CTRL_SEQUENCE_DIR_DOWN_KEY         '\033' + '[' + 'B'
        #define MYSH_CTRL_SEQUENCE_DIR_RIGHT_KEY        '\033' + '[' + 'C'
        #define MYSH_CTRL_SEQUENCE_DIR_LEFT_KEY         '\033' + '[' + 'D'
        #define MYSH_CTRL_SEQUENCE_DIR_BYTES            3
    #endif

    #if !defined(__C51__)
        #if (MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD == MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD_2)
            #define MYSH_CTRL_SEQUENCE_DIR_UP_KEY           '\xe0' + '\x48'
            #define MYSH_CTRL_SEQUENCE_DIR_DOWN_KEY         '\xe0' + '\x50'
            #define MYSH_CTRL_SEQUENCE_DIR_RIGHT_KEY        '\xe0' + '\x4d'
            #define MYSH_CTRL_SEQUENCE_DIR_LEFT_KEY         '\xe0' + '\x4b'
            #define MYSH_CTRL_SEQUENCE_DIR_BYTES            2
        #endif
    #endif
#endif






/** 特殊输出终端控制序列定义 **/
/* 颜色控制定义 */
#define MYSH_PRINT_RESET_COLOR()            "00"            /** 重置前景色 **/

#define MYSH_PRINT_BLACK_FORE_COLOR()       "30"            /** 绘制黑色前景色 **/
#define MYSH_PRINT_RED_FORE_COLOR()         "31"            /** 绘制红色前景色 **/
#define MYSH_PRINT_GREEN_FORE_COLOR()       "32"            /** 绘制绿色前景色 **/
#define MYSH_PRINT_YELLOW_FORE_COLOR()      "33"            /** 绘制黄色前景色 **/
#define MYSH_PRINT_BLUE_FORE_COLOR()        "34"            /** 绘制蓝色前景色 **/
#define MYSH_PRINT_MAGENTA_FORE_COLOR()     "35"            /** 绘制品红色前景色 **/
#define MYSH_PRINT_CYAN_FORE_COLOR()        "36"            /** 绘制青色前景色 **/
#define MYSH_PRINT_WHITE_FORE_COLOR()       "37"            /** 绘制白色前景色 **/

#define MYSH_PRINT_BLACK_BACK_COLOR()       "40"            /** 绘制黑色背景色 **/
#define MYSH_PRINT_RED_BACK_COLOR()         "41"            /** 绘制红色背景色 **/
#define MYSH_PRINT_GREEN_BACK_COLOR()       "42"            /** 绘制绿色背景色 **/
#define MYSH_PRINT_YELLOW_BACK_COLOR()      "43"            /** 绘制黄色背景色 **/
#define MYSH_PRINT_BLUE_BACK_COLOR()        "44"            /** 绘制蓝色背景色 **/
#define MYSH_PRINT_MAGENTA_BACK_COLOR()     "45"            /** 绘制品红色背景色 **/
#define MYSH_PRINT_CYAN_BACK_COLOR()        "46"            /** 绘制青色背景色 **/
#define MYSH_PRINT_WHITE_BACK_COLOR()       "47"            /** 绘制白色背景色 **/

/** 是否提高颜色亮度 **/
#define MYSH_IS_COLOR_BRIGHTER      1
#if MYSH_IS_COLOR_BRIGHTER
    #define MYSH_COLOR_BRIGHTER "1"
#else
    #define MYSH_COLOR_BRIGHTER "0"
#endif

/* 终端错误颜色 */
#define MYSH_SHELL_ERR_COLOR()              "\033[" MYSH_COLOR_BRIGHTER ";" MYSH_PRINT_RED_FORE_COLOR() "m"
/* 终端提示符颜色 */
#define MYSH_SHELL_INFO_COLOR()             "\033[" MYSH_COLOR_BRIGHTER ";" MYSH_PRINT_GREEN_FORE_COLOR() "m"
/* 终端提示符颜色 */
#define MYSH_SHELL_WARNING_COLOR()          "\033[" MYSH_COLOR_BRIGHTER ";" MYSH_PRINT_YELLOW_FORE_COLOR() "m"
/* 重置终端颜色 */
#define MYSH_SHELL_RESET_COLOR()            "\033[" MYSH_PRINT_RESET_COLOR() "m"


/** VT100 兼容终端控制序列 **/
/* 清屏指令 */
#define MYSH_SHELL_CTRL_SCREEN_CLEAR()          "\033[2J\033[H"
/* 保存终端光标当前的位置 */
#define MYSH_SHELL_CTRL_STORE_CURSOR()          "\033[s"
/* 恢复终端光标保存的位置 */
#define MYSH_SHELL_CTRL_RESTORE_CURSOR()        "\033[u"
/* 清除光标之后所显示所有字符 */
#define MYSH_SHELL_CTRL_CLEAR_AFTER_CURSOR()    "\033[K"
/* 光标向右移 n 格 */
#define MYSH_SHELL_CTRL_MOVE_CURSOR_RIGHT(n)    "\033[" MYSH_NBR_TO_STR(n) "C"
/* 光标向左移 n 格 */
#define MYSH_SHELL_CTRL_MOVE_CURSOR_LEFT(n)     "\033[" MYSH_NBR_TO_STR(n) "D"

/** 检查字符是否在 0x00 ~ 0x7f 之间 **/
#define MYSH_IS_CHAR_ASCII(ch)      (!(ch & 0x80))



#ifdef __cplusplus
}
#endif

#endif // MYSH_CONF_H







































