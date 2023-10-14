#include "stdio.h"
#include "main.h"

#define USING_USER_CMD		1

#if USING_USER_CMD
static void user_exec(int argc, char* argv[])
{
	int i = 0; 
	for(i = 0; i < argc; ++i)
	{
		mysh_put_string(argv[i]);
		mysh_put_string("\r\n");
	}
}
#endif


void main(void)
{	
	EA = 1;
	dev_timer0_init();
	dev_uart1_init();


	mysh_init();
	
	#if USING_USER_CMD
	{
		static struct mysh_cmd_dsc user_dsc;
		user_dsc.name 	= "user";
		user_dsc.man 	= "test for user.";
		user_dsc.exec 	= user_exec;
		mysh_add_cmd(&user_dsc);
	}
	#endif
	
	while(1)
	{
		mysh_task_handler();
	}

}