/**
 * @file    mysh.c
 * @author  Southern Sandbox
 * @brief   mysh 的具体实现
 * @version 0.1
 * @date    2023-10-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mysh.h"

/**
 * @brief mysh 管理器
 * 
 * @note mysh_mgr 主要由如下几个结构体成员：
 *          · mysh_rd       用于管理普通输入字符的存储、字符数量和光标的位置
 *          · mysh_cseq     用于管理终端控制序列的输入
 *          · mysh_flag     用于管理标志位
 *          · mysh_cmd      用于管理命令和命令行的参数
 *          · mysh_opt      用于管理一些选项功能
 */
struct mysh_mgr
{
    /** 读入信息 **/
    struct mysh_rd
    {
        char buf[MYSH_RX_BUF_MAX + 1];  /** 接收缓冲区，多留出的一个字节用于存储 '\0' **/
        mysh_rd_cnt_t count;            /** 读取计数 **/
        mysh_rd_cnt_t cursor_pos;       /** 光标偏移量，始终为正值，计算方式: cursor 位置 与 rd.buf[rd.count] 之间的字符数 **/
    } rd;

    #if MYSH_SUPPORT_DIR_KEY
    /** 控制序列信息 **/
    struct mysh_cseq
    {
        char buf;               /** 终端控制序列缓冲区 **/
        mysh_uint8_t count;     /** 读取计数 **/

    } cseq;
    #endif

    /** 标志位集 **/
    union mysh_flag
    {
        struct mysh_flag_set
        {
            mysh_flag_val_t is_print_shell_info     : 1;    /** 是否打印了终端信息 **/
            mysh_flag_val_t is_endl                 : 1;    /** 是否接收到了结束字符 **/
            mysh_flag_val_t is_cseq                 : 1;    /** 是否接收到了终端控制序列 **/
        } set;
        mysh_flag_val_t all;                                /** 该成员用于将全部 flag 进行快速赋值 **/
    } flag;     /** 标志位集 **/

    /** 指令信息 **/
    struct mysh_cmd
    {
        const struct mysh_cmd_dsc* list[MYSH_CMD_COUNT_MAX];
        mysh_cmd_cnt_t list_count;
        struct mysh_cmd_arg
        {
            char    c;                       /** 参数数量 **/
            char*   v[MYSH_ARGC_MAX];        /** 参数数组 **/
        } arg;
    } cmd;

    /** mysh 选项 **/
    union mysh_opt
    {
        struct mysh_opt_item
        {
            mysh_opt_val_t echo     : 2;    /** 回显选项 **/
        } item;
        mysh_opt_val_t all;
    } opt;

};
static struct mysh_mgr mgr;






/** ********************************* **
 *                                     *
 *             Porting API             *
 *          for Input / Output         *
 *                                     *
 ** ********************************* **/
extern char mysh_get_char (void);
extern void mysh_put_char (char ch);
#if MYSH_SUPPORT_PRINT_TIME_INFO
extern void mysh_get_time_data (mysh_uint8_t hms[3]);
#endif

void mysh_put_echo (char ch);
void mysh_put_string (const char* s);
#if MYSH_SUPPORT_PRINT_TIME_INFO
void mysh_put_time_info (void);
#endif


void mysh_put_echo(char ch)
{
    /** 字符回显 **/
    switch(mgr.opt.item.echo)
    {
    case 0: mysh_put_char(ch); break;
    case 1: break;
    case 2: mysh_put_char('*'); break;
    }
}

/**
 * @brief 输出一串字符串
 */
void mysh_put_string(const char* s)
{
    for( ; *s != '\0'; s++)
        mysh_put_echo(*s);
}

/**
 * @brief 输出终端控制序列
 */
#define mysh_put_shell_ctrl(word)   mysh_put_string(word)

/**
 * @brief 输出终端彩色控制序列
 */
#if MYSH_SUPPORT_PRINT_MULTI_COLOR
    #define mysh_put_color_ctrl(word)   mysh_put_shell_ctrl(word)
#else
    #define mysh_put_color_ctrl(word)
#endif

/**
 * @brief 输出时间信息
 */
#if MYSH_SUPPORT_PRINT_TIME_INFO
void mysh_put_time_info(void)
{
    char buf[12] = "[00:00:00] \0";
    mysh_uint8_t hms[3], i;
    mysh_get_time_data(hms);

    for(i = 1 ; i <= 7; i += 3)
    {
        buf[(mysh_uint8_t) i]      += hms[i / 3] / 10;
        buf[(mysh_uint8_t) i + 1]  += hms[i / 3] % 10;
    }

    mysh_put_string(buf);
}
#else
    #define mysh_put_time_info()
#endif








/** ********************************* **
 *                                     *
 *            String Function          *
 *                                     *
 ** ********************************* **/
static int _mysh_str_cmp (const char* s1, const char* s2);
static char* _mysh_str_tok (char* s, const char delim);


/**
 * @brief 比较两个字符串是否相等
 *
 * @param s1
 * @param s2
 * @return 0: 字符串相等，-1: 字符串不相等
 */
static int _mysh_str_cmp(const char* s1, const char* s2)
{
    for( ; (*s1 != '\0') && (*s1 == *s2); s1++, s2++);
    return -(*s1 != *s2);
}


/**
 * @brief 将字符串以分隔符 delim 进行分割，并返回分割后字符串的地址
 *
 * @warning 该函数将修改原字符串，注意时请注意
 *
 * @note 本函数采用偏移指针 s 自身的方式来来查找和替换字符，这样做是为了缩减最后编译后 code 的大小
 *
 * @param s1    原字符串
 * @param delim 分隔符
 *
 * @return  NULL: 该字符串不存在 delim
 *          其他: 子字符串首地址
 */
static char* _mysh_str_tok (char* s, const char delim)
{
    static char* next = NULL;
    mysh_rd_cnt_t i = 0;

    /** 判定字符串的合法性 **/
    if(s == NULL)
    {
        if(next == NULL)
            return NULL;
        s = next;
    }

    /** 遍历查找分隔符 **/
    for( ; *s != '\0' && *s != delim; ++s, ++i);

    /** 将分隔符替换为 '\0' **/
    for( ; *s == delim; ++s, ++i)
        *s = '\0';

    /** 重置 next 指针位置，若下一个字符串存在则令 next 指向下一个字符串，否则重新指向 NULL **/
    if(*s != '\0')
        next = s;
    else
        next = NULL;

    return s - i;
}












#if MYSH_SUPPORT_DIR_KEY
/** ********************************* **
 *                                     *
 *              Static API             *
 *         for Control Sequence        *
 *                                     *
 ** ********************************* **/
static void _mysh_mgr_cseq_clear (void);
#if !MYSH_SUPPORT_DIR_KEY_FILTER
static void _mysh_cseq_cursor_move_left (void);
static void _mysh_cseq_cursor_move_right (void);
static void _mysh_remove_print_char (void);
static void _mysh_insert_print_char (char ch);
#endif
static void _mysh_push_cseq_char (char ch);


/**
 * @brief 终端控制序列重置
 */
static void _mysh_mgr_cseq_clear(void)
{
    memset(&mgr.cseq, 0, sizeof(mgr.cseq));
    mgr.flag.set.is_cseq = 0;
}

#if !MYSH_SUPPORT_DIR_KEY_FILTER
/**
 * @brief 执行光标左移操作
 */
static void _mysh_cseq_cursor_move_left(void)
{
    if(mgr.rd.cursor_pos < mgr.rd.count)
    {
        mgr.rd.cursor_pos++;
        mysh_put_char('\b');
    }
}


/**
 * @brief 执行光标右移操作
 */
static void _mysh_cseq_cursor_move_right(void)
{
    if(mgr.rd.cursor_pos > 0)
    {
        mgr.rd.cursor_pos--;
        mysh_put_shell_ctrl( MYSH_SHELL_CTRL_MOVE_CURSOR_RIGHT(1) );
    }
}


/**
 * @brief 以光标当前所在位置，移除一个已打印字符，并重新打印 rd.buf
 * 
 * @note 该函数仅在光标位置不位于用户输入的最后一个字符时使用
 */
static void _mysh_remove_print_char(void)
{
    mysh_rd_cnt_t ofs = mgr.rd.count - mgr.rd.cursor_pos;
    mysh_rd_cnt_t i = 0;

    /** 当光标指向当前输入的最开端时，退格不再生效 **/
    if(!ofs)
        return;

    /** 先退格一步，保存当前光标的位置，并清空光标后面所有字符 **/
    mysh_put_shell_ctrl( "\b"
                         MYSH_SHELL_CTRL_STORE_CURSOR()
                         MYSH_SHELL_CTRL_CLEAR_AFTER_CURSOR() );

    /** 在内存上移动光标后面的字符 @note 为避免破坏内存，此处的赋值是逐字节进行的 **/
    for(i = ofs; i < ofs + mgr.rd.cursor_pos; ++i)
        mgr.rd.buf[i - 1] = mgr.rd.buf[i];
    mgr.rd.buf[mgr.rd.count - 1] = '\0';

    /** 打印后面的字符 **/
    mysh_put_string(mgr.rd.buf + ofs - 1);

    /** 光标归位 **/
    mysh_put_shell_ctrl( MYSH_SHELL_CTRL_RESTORE_CURSOR() );

    /** 重新更新 rd 数据 **/
    mgr.rd.count--;
}


/** 以光标当前所在位置，插入一个新的字符，并重新打印 rd.buf **/
static void _mysh_insert_print_char(char ch)
{
    mysh_rd_cnt_t ofs = mgr.rd.count - mgr.rd.cursor_pos;
    mysh_rd_cnt_t i = 0;

    /** 保存当前光标的位置，并清空光标后面所有字符 **/
    mysh_put_shell_ctrl( MYSH_SHELL_CTRL_STORE_CURSOR()
                         MYSH_SHELL_CTRL_CLEAR_AFTER_CURSOR() );

    /** 在内存上移动光标后面的字符 **/
    for(i = mgr.rd.count; i > ofs; --i)
         mgr.rd.buf[i] = mgr.rd.buf[i - 1];
    mgr.rd.buf[mgr.rd.count + 1] = '\0';

    /** 将新字符插入到字符串中 **/
    mgr.rd.buf[ofs] = ch;

    /** 打印后面的字符 **/
    mysh_put_string(mgr.rd.buf + ofs);

    /** 光标归位，并向右/后移动一格 **/
    mysh_put_shell_ctrl( MYSH_SHELL_CTRL_RESTORE_CURSOR()
                         MYSH_SHELL_CTRL_MOVE_CURSOR_RIGHT(1) );

    /** 重新更新 rd 数据 **/
    mgr.rd.count++;
}
#endif


/**
 * @brief 将一个普通字符推入到控制序列缓存，并控制光标移动
 *
 * @param ch
 */
static void _mysh_push_cseq_char (char ch)
{
    /** 通过求和方式储存控制序列 **/
    mgr.cseq.buf += ch;
    mgr.cseq.count++;

    /** 控制序列查表 **/
    switch (mgr.cseq.buf)
    {
    case MYSH_CTRL_SEQUENCE_DIR_UP_KEY:     /** 上 **/
    case MYSH_CTRL_SEQUENCE_DIR_DOWN_KEY:   /** 下 **/
    #ifdef MYSH_SUPPORT_DIR_KEY
        // ...
    #endif
        break;

    case MYSH_CTRL_SEQUENCE_DIR_RIGHT_KEY:  /** 右 **/
    #ifdef MYSH_SUPPORT_DIR_KEY
        #if !MYSH_SUPPORT_DIR_KEY_FILTER
        _mysh_cseq_cursor_move_right();
        #endif
    #endif
        break;

    case MYSH_CTRL_SEQUENCE_DIR_LEFT_KEY:   /** 左 **/
    #ifdef MYSH_SUPPORT_DIR_KEY
        #if !MYSH_SUPPORT_DIR_KEY_FILTER
        _mysh_cseq_cursor_move_left();
        #endif
    #endif
        break;

    default:
        break;
    }

    if(mgr.cseq.count == MYSH_CTRL_SEQUENCE_DIR_BYTES)
        _mysh_mgr_cseq_clear();
}
#endif






/** ********************************* **
 *                                     *
 *              Static API             *
 *                                     *
 ** ********************************* **/
static void _mysh_mgr_rd_clear          (void);
static void _mysh_print_shell_info      (void);
static void _mysh_print_backpace        (void);
static void _mysh_str_split             (char* s);
static void _mysh_push_normal_char      (char ch);
#if MYSH_SUPPORT_TAB_COMPETION
static void _mysh_cmd_completion        (void);
#endif
static void _mysh_get_input             (void);
static void _mysh_cmd_search_and_exec   (void);





/**
 * @brief 重置管理器
 */
static void _mysh_mgr_rd_clear(void)
{
    /** 读取重置 **/
    memset(&mgr.rd, 0, sizeof(mgr.rd));

    #if MYSH_SUPPORT_DIR_KEY
    /** 终端控制序列重置 **/
    _mysh_mgr_cseq_clear();
    #endif

    /** 标志位重置 **/
    mgr.flag.all = 0;

    /** 参数重置 **/
    memset(&mgr.cmd.arg, 0, sizeof(mgr.cmd.arg));
}


/**
 * @brief 打印终端提示信息
 */
static void _mysh_print_shell_info (void)
{
    mysh_put_color_ctrl(MYSH_SHELL_INFO_COLOR());
    mysh_put_time_info();
    mysh_put_string("mysh > ");
    mysh_put_color_ctrl(MYSH_SHELL_RESET_COLOR());
}


/**
 * @brief 在终端输出退格键，同时实现字符删除效果
 * 
 * @note 该函数仅在光标位于用户输入字符串的末尾时才会调用
 */
static void _mysh_print_backpace (void)
{
    if(mgr.rd.count > 0)
    {
        mgr.rd.buf[--mgr.rd.count] = '\0';
        mysh_put_string("\b \b");
    }
}


/**
 * @brief 将目标字符串分割为若干 token 子字符串，token 的地址将被存储到 mgr 的 argv[] 中
 * @note  该函数将破坏原有的字符串
 * @param s
 */
static void _mysh_str_split (char* s)
{
    char* token = _mysh_str_tok(s, MYSH_DELIM_CHAR);
    mgr.cmd.arg.c = 0;

    if(token != NULL)
    {
        memset(mgr.cmd.arg.v, 0, sizeof(mgr.cmd.arg.v));
        mgr.cmd.arg.v[0] = token;
    }
    else
        return;

    while(token != NULL)
    {
        if(++mgr.cmd.arg.c >= MYSH_ARGC_MAX)
            break;

        token = _mysh_str_tok(NULL, MYSH_DELIM_CHAR);
        mgr.cmd.arg.v[(mysh_uint8_t) mgr.cmd.arg.c] = token;
    }
}




/**
 * @brief 将一个普通字符推入到缓冲区，并回显这个字符
 * @param ch
 */
static void _mysh_push_normal_char (char ch)
{
    /** 其余的字符将会被进一步处理 **/
    mysh_put_echo(mgr.rd.buf[mgr.rd.count++] = ch);
}


#if MYSH_SUPPORT_TAB_COMPETION
/**
 * @brief 命令名称补全
 */
static void _mysh_cmd_completion (void)
{
    mysh_cmd_cnt_t i = 0;
    mysh_cmd_cnt_t cpt_cnt = 0;                 /** 符合补全的指令数 **/
    const struct mysh_cmd_dsc* only = NULL;     /** 该指针仅在只有一个指令符合补全时指向那个指令描述对象 **/

    /** 跳过空接收 **/
    if(!mgr.rd.count)
        return;

    /** 遍历指令表，查找符合补全要求的指令( name len > rd count )并打印 **/
    for(i = 0; i < mgr.cmd.list_count; ++i)
    {
        const struct mysh_cmd_dsc* dsc = mgr.cmd.list[i];
        mysh_rd_cnt_t j = 0;

        /** 遍历字符 **/
        for(j = 0; dsc->name[j] == mgr.rd.buf[j] && dsc->name[j] != '\0'; ++j);

        /** 排除：j != rd_count，即符合补全要求的指令应当包含 rd.buf 中所有的字符 **/
        if(j != mgr.rd.count)
            continue;

        /** 将当前匹配成功的命令名称进行打印 **/
        mysh_put_string("\r\n");
        mysh_put_string(dsc->name);
        cpt_cnt++;
        only = dsc;
    }

    /** 依据匹配结果进行区分显示 **/
    switch (cpt_cnt)
    {
    case 1: /** 当仅有一个指令符合补全时，将自动将命令名称填充到 rd.buf 并重新打印命令 **/
        {
            mysh_rd_cnt_t len = strlen(only->name);
            mysh_put_string("\r\n");                /** 换行 **/
            _mysh_print_shell_info();               /** 模拟打印提示符信息 **/
            mysh_put_string(only->name);            /** 打印指令名称 **/
            memcpy(mgr.rd.buf, only->name, len);    /** 重置 rd.buf **/
            mgr.rd.count = len;
        }
        break;

    default:
        _mysh_mgr_rd_clear();       /** 重置 rd 接收 **/
        mgr.flag.set.is_endl = 1;   /** 强制换行结束当前接收 **/
        break;
    }
}
#endif


/**
 * @brief 终端接收来自用户的输入，并执行相应的操作
 */
static void _mysh_get_input (void)
{
    char ch = mysh_get_char();

   switch (ch)
    {
    case 0:
        return;

    case 0x7f:
    case '\b':
        #if MYSH_SUPPORT_DIR_KEY && !MYSH_SUPPORT_DIR_KEY_FILTER
        if(!mgr.rd.cursor_pos)
            _mysh_print_backpace();
        else
            _mysh_remove_print_char();
        #else
        _mysh_print_backpace();
        #endif

        return;

    #if MYSH_SUPPORT_DIR_KEY
    case MYSH_CTRL_SEQUENCE_DIR_CHAR_HEAD:
        mgr.flag.set.is_cseq = 1;
        mgr.cseq.buf += ch;
        mgr.cseq.count++;
        break;
    #endif

    case '\t':
        #if MYSH_SUPPORT_TAB_COMPETION
        _mysh_cmd_completion();
        #endif
        return ;

    case '\r':
    case '\n':
        mgr.flag.set.is_endl = 1;
        return;

    default:
        #if MYSH_SUPPORT_DIR_KEY
        if(!mgr.flag.set.is_cseq)
        {
            /** 检查是否越界 **/
            if(mgr.rd.count >= MYSH_RX_BUF_MAX)
                break;

            #if !MYSH_SUPPORT_DIR_KEY_FILTER
            /** 推入 char **/
            if(!mgr.rd.cursor_pos)
                _mysh_push_normal_char(ch);
            else
                _mysh_insert_print_char(ch);
            #else
            _mysh_push_normal_char(ch);
            #endif
        }
        else
            _mysh_push_cseq_char(ch);       /** 将字符推入控制序列缓冲区 **/
        #else
        if(mgr.rd.count < MYSH_RX_BUF_MAX)
            _mysh_push_normal_char(ch);
        #endif
        break;
    }
}


/**
 * @brief 查询目标指令并执行
 */
static void _mysh_cmd_search_and_exec(void)
{
    mysh_cmd_cnt_t idx = 0;

    if(!mgr.cmd.arg.c)
        return;

    for(idx = 0; idx < mgr.cmd.list_count; ++idx)
    {
        const struct mysh_cmd_dsc* dsc = mgr.cmd.list[idx];

        /** 命令名比较 **/
        if(!_mysh_str_cmp(dsc->name, mgr.cmd.arg.v[0]))
        {
            /** 执行命令 **/
            dsc->exec(mgr.cmd.arg.c, mgr.cmd.arg.v);
            return;
        }
    }

    mysh_put_color_ctrl(MYSH_SHELL_ERR_COLOR());
    mysh_put_string("\"");
    mysh_put_string(mgr.cmd.arg.v[0]);
    mysh_put_string("\" is not found.\r\n");
    mysh_put_color_ctrl(MYSH_SHELL_RESET_COLOR());
}




/** ********************************* **
 *                                     *
 *        Inner Command Function       *
 *                                     *
 ** ********************************* **/
#if MYSH_SUPPORT_INNER_COMMANDS
#if MYSH_SUPPORT_INNER_COMMANDS_HELP
    static void help_exec (void);
#endif
#if MYSH_SUPPORT_INNER_COMMANDS_VER
    static void ver_exec (void);
#endif
#if MYSH_SUPPORT_INNER_COMMANDS_VIS
static void vis_exec(int argc, char* argv[]);
#endif
#if MYSH_SUPPORT_INNER_COMMANDS_CLEAR
static void clear_exec(void);
#endif



#if MYSH_SUPPORT_INNER_COMMANDS_HELP
static void help_exec(void)
{
    int idx = 0;
    for(idx = 0; idx < mgr.cmd.list_count; ++idx)
    {
        const struct mysh_cmd_dsc* dsc = mgr.cmd.list[idx];
        mysh_put_string(dsc->name);
        mysh_put_string("\t - ");
        mysh_put_string(dsc->man);
        mysh_put_string("\r\n");
    }
}
#endif

#if MYSH_SUPPORT_INNER_COMMANDS_VER
static void ver_exec(void)
{
    mysh_put_string(MYSH_VERSION_STRING(MYSH_VERSION_TYPE, MYSH_VERSION_MAJOR, MYSH_VERSION_MINOR) "\r\n");
}
#endif

#if MYSH_SUPPORT_INNER_COMMANDS_VIS
static void vis_exec(int argc, char* argv[])
{
    if(argc > 1)
        mysh_set_echo(!(*argv[1] - '0'));
}
#endif

#if MYSH_SUPPORT_INNER_COMMANDS_CLEAR
static void clear_exec(void)
{
    mysh_put_shell_ctrl( MYSH_SHELL_CTRL_SCREEN_CLEAR() );
}
#endif


static void _mysh_inner_cmd_register(void)
{
    #if MYSH_SUPPORT_INNER_COMMANDS_HELP
    {
        /** help **/
        static struct mysh_cmd_dsc help_dsc;
        help_dsc.name = "help";
        help_dsc.man = "list all commands.";
        help_dsc.exec = (mysh_cmd_exec) help_exec;
        mysh_add_cmd(&help_dsc);
    }
    #endif

    #if MYSH_SUPPORT_INNER_COMMANDS_VER
    {
        /** ver **/
        static struct mysh_cmd_dsc ver_dsc;
        ver_dsc.name = "ver";
        ver_dsc.man = "show version information";
        ver_dsc.exec = (mysh_cmd_exec) ver_exec;
        mysh_add_cmd(&ver_dsc);
    }
    #endif

    #if MYSH_SUPPORT_INNER_COMMANDS_VIS
    {
        /** visible **/
        static struct mysh_cmd_dsc vis_dsc;
        vis_dsc.name = "vis";
        vis_dsc.man = "set output visible. 0: unvisible, 1: visible.";
        vis_dsc.exec = vis_exec;
        mysh_add_cmd(&vis_dsc);
    }
    #endif

    #if MYSH_SUPPORT_INNER_COMMANDS_CLEAR
    {
        /** clear **/
        static struct mysh_cmd_dsc clear_dsc;
        clear_dsc.name = "clear";
        clear_dsc.man = "clear the shell output and reset cursor's position.";
        clear_dsc.exec = (mysh_cmd_exec) clear_exec;
        mysh_add_cmd(&clear_dsc);
    }
    #endif
}
#endif







/** ********************************* **
 *                                     *
 *              Public API             *
 *                                     *
 ** ********************************* **/
void mysh_init (void);
void mysh_task_handler (void);
int  mysh_add_cmd (const struct mysh_cmd_dsc* dsc);
void mysh_print (const char* s);



void mysh_init (void)
{
    /** 初始化管理器 **/
    memset(&mgr, 0, sizeof(struct mysh_mgr));

    /** 设置输出可视化 **/
#if MYSH_SUPPORT_OUTPUT_UNVISIBLE_IN_INIT
    mgr.opt.item.echo = 1;
#endif

    /** 注册内部指令 **/
#if MYSH_SUPPORT_INNER_COMMANDS
    _mysh_inner_cmd_register();
#endif
}


void mysh_task_handler (void)
{
    /** 打印终端提示信息 **/
    if(mgr.flag.set.is_print_shell_info == 0)
    {
        mgr.flag.set.is_print_shell_info = 1;
        _mysh_print_shell_info();
    }
	
    /** 获取接收数据 **/
    _mysh_get_input();

    /** 解析接收的数据 **/
    if(mgr.flag.set.is_endl)
    {
        /** 换行打印 **/
        mysh_put_string("\r\n");

        /** 检查接收字符数量 **/
        if(mgr.rd.count)
        {
            /** 将字符串进行拆分 **/
            _mysh_str_split(mgr.rd.buf);

            /** 查询目标指令并执行 **/
            _mysh_cmd_search_and_exec();
        }

        /** 重置管理器读取有关的变量 **/
        _mysh_mgr_rd_clear();
    }
}


int mysh_add_cmd (const struct mysh_cmd_dsc* dsc)
{
    if(mgr.cmd.list_count < MYSH_CMD_COUNT_MAX)
    {
        mgr.cmd.list[mgr.cmd.list_count++] = dsc;
        return 0;
    }
    return -1;
}

void mysh_set_echo (mysh_opt_val_t opt)
{
    mgr.opt.item.echo = opt;
}


















