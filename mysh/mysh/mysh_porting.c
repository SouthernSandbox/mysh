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
// your headers

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
    char ch = 0;
    // get a character, such as: dev_uart1_read(&ch, sizeof(ch));
    // your code
    return ch;
}



/**
 * @brief 将 mysh 提供的一个字符进行输出
 * 
 * @param ch 
 */
void mysh_put_char(char ch)
{
    // put a character, such as: dev_uart1_write(&ch, sizeof(ch));
    // your code
}



#if MYSH_SUPPORT_PRINT_TIME_INFO
/**
 * @brief 向 mysh 提供时间（时、分、秒）数据
 * 
 * @note hms[]：[0] - hour, [1] - min, [0] - sec
 * 
 * @param hms 
 */
void mysh_get_time_data(mysh_uint8_t hms[3])
{
    // your code
}
#endif

#ifdef __cplusplus
}
#endif
