#include "signal_handle.h"
#include <stdio.h>
#include <signal.h>
bool SIGNAL_RECIEVED = false;

void sig_handler(int signo)
{
	if( signo == SIGINT )
	{
		printf("received SIGINT\n");
		SIGNAL_RECIEVED = true;
	}
}



