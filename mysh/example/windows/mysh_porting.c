/**
 * @file    mysh_porting.c
 * @author  Southern Sandbox
 * @brief   提供用于移植的接口函数的实现
 * @version 0.1
 * @date    2023-10-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mysh.h"
#include "stdio.h"
#include "conio.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 向 mysh 提供一个从用户端输入的字符
 * 
 * @note 请注意，当未读取到字符时，请确保函数返回的值为 0x0
 * 
 * @return char 
 */
char mysh_get_char(void)
{
    return _getch();
}


/**
 * @brief 将 mysh 提供的一个字符进行输出
 * 
 * @param ch 
 */
void mysh_put_char(char ch)
{
    putchar(ch);
}



#if MYSH_SUPPORT_PRINT_TIME_INFO
/**
 * @brief 向 mysh 提供时间（时、分、秒）数据
 * 
 * @note hms[]：[0] - hour, [1] - min, [0] - sec
 * 
 * @param hms 
 */
void mysh_get_time_data(char* hms)
{
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);

    hms[0] = (char) tm->tm_hour;
    hms[1] = (char) tm->tm_min;
    hms[2] = (char) tm->tm_sec;
}
#endif

#ifdef __cplusplus
}
#endif
