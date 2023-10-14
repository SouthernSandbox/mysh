/**
 * @file    mysh.h
 * @author  Southern Sandbox
 * @brief   mysh 用户引用的头文件，为用户提供了基本的 API 和版本号
 * 
 * @details · mysh 是一个轻量级的命令行程序，为用户提供一套命令行操作的接口，用于调试或者是信息查看。
 *          · mysh 最初的目标是为 8051 单片机提供简易的 shell 功能，因此采用兼容 C51 的 C89 标准，以确保对 8051 平台的兼容。
 *          · mysh 为了能在 8051 平台上保持较小的代码了和内存消耗，采用全静态创建数组的方式（包括命令数组、参数数组等。对于数组大小，需要在 mysh_conf.h 中进行修改），
 *            同时在 mysh_conf.h 中提供了可修改的类型，而函数的也尽量减少参数的传递或无用的返回值；
 *            mysh 也没有额外的启动提示信息。
 *          · mysh 支持功能裁剪，在基本功能的基础上，添加了可裁剪的增强功能：
 *              基本功能：
 *              1. 可移植的字符输入与输出
 *              2. 自定义命令的添加、命令的执行和命令行参数的传递（ @note 命令添加时，需要确保 mysh_cmd_dsc 对象是 static 的或者是全局的）
 *              3. 输入时支持 backpace 字符删除操作
 *              增强功能：
 *              1. 彩色字体输出（时间、提示符和错误信息）
 *              2. 可设置输出和回显字符不可见
 *              3. TAB 键命令名补全
 *              4. 最多 4 个的内置指令（help/ver/clear/vis）
 *              5. 方向键控制（包括命令行光标位移、字符的插入和移除）和过滤功能
 *              6. 输出时间信息（需要对可移植的时间获取 API 进行重新实现）
 *          · mysh 的命令行控制兼容 VT100 终端控制序列，请确保使用时 PC 控制台支持 VT100 控制码的功能，否则可能导致输出内容异常
 *          · mysh 虽然最初是为 8051 单片机编写的 shell，但也可以方便的移植到其他的平台，例如：
 *              1. 其他内核的单片机
 *              2. Windows (由 Qt 创建的 C 项目)
 *              3. Android 的 Cxxdroid 等
 *            mysh 由 4 个文件组成：
 *              1. mysh.h               用户需要引用的头文件
 *              2. mysh.c               mysh 的具体实现
 *              3. mysh_conf.h          mysh 的配置文件
 *              4. mysh_porting.c       mysh 的输入输出、时间获取的可移植 API 定义文件， @note 用户只需要重新实现这些 API 即可移植到不同的平台上  
 *            在 main.c 文件中，#include "mysh.h"，然后在 main() 中调用 mysh_init()、mysh_task_handler() 即可运行， @note mysh_task_handler() 需要运行在 while(1) 中，以便及时获取用户输入。
 *         
 * @version 0.1
 * @date    2023-10-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef MYSH_H
#define MYSH_H

#include "string.h"     /** for memset() **/

#ifdef __cplusplus
extern "C" {
#endif

#include "mysh_conf.h"


/** 版本号 **/
#define MYSH_VERSION_TYPE       "alpha"
#define MYSH_VERSION_MAJOR      1
#define MYSH_VERSION_MINOR      0
#define MYSH_VERSION_STRING(type, major, minor)     \
    type " v" MYSH_NBR_TO_STR(major) "." MYSH_NBR_TO_STR(minor)




/**
 * @brief mysh 命令描述结构体
 */
struct mysh_cmd_dsc
{
    char* name;             /** 命令名称 **/
    char* man;              /** 命令描述 **/
    mysh_cmd_exec exec;     /** 命令执行函数 **/
};


/**
 * @brief 必要 API
 */
void mysh_init          (void);     /** mysh 初始化函数 **/
void mysh_task_handler  (void);     /** mysh 任务函数 @note 该函数一般放置在 while(1) 以便及时读取用户输入 **/

/**
 * @brief 其他 API
 */
int  mysh_add_cmd       (const struct mysh_cmd_dsc* dsc);   /** 添加命令 **/
void mysh_put_string    (const char* s);                    /** 输出一个字符串 **/
void mysh_set_echo      (mysh_opt_val_t opt);               /** 设置输出回显模式 **/



#ifdef __cplusplus
}
#endif

#endif	// MYSH_H
