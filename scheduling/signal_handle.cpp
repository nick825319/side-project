#include "signal_handle.h"
#include <stdio.h>
#include <signal.h>


void sig_handler(int signo)
{
	if( signo == SIGINT )
	{
		printf("received SIGINT\n");
		
	}
}



