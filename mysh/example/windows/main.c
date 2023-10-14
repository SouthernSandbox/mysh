#include "stdio.h"
#include "stdbool.h"
#include "mysh.h"

#define USING_USER_CMD		1

static bool is_mysh_quit = false;

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

static void quit_exec(void)
{
	is_mysh_quit = true;

	mysh_put_string(MYSH_SHELL_WARNING_COLOR());
	mysh_put_string("mysh: quit.\r\n");
	mysh_put_string(MYSH_SHELL_RESET_COLOR());
}
#endif



int main(void)
{	
	mysh_init();
	
	#if USING_USER_CMD
	{
		static struct mysh_cmd_dsc user_dsc;
		user_dsc.name 	= "user";
		user_dsc.man 	= "test for user.";
		user_dsc.exec 	= user_exec;
		mysh_add_cmd(&user_dsc);
	}

	{
		static struct mysh_cmd_dsc quit_dsc;
		quit_dsc.name 	= "quit";
		quit_dsc.man 	= "quit the mysh.";
		quit_dsc.exec 	= (mysh_cmd_exec) quit_exec;
		mysh_add_cmd(&quit_dsc);
	}
	#endif
	
	while(!is_mysh_quit)
	{
		mysh_task_handler();
	}

	return 0;
}