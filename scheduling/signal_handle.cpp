#include "signal_handle.h"
#include <stdio.h>
#include <signal.h>
extern bool signal_recieved = false;

void sig_handler(int signo)
{
	if( signo == SIGINT )
	{
		printf("received SIGINT\n");
		signal_recieved = true;
	}
}



