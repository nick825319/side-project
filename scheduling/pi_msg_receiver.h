#ifndef PI_MSG_RECEIVER_H
#define PI_MSG_RECEIVER_H
#include <string>
#include "scheduling.h"

typedef struct{
    char* ipAddress;
	char* PiIpAddress;
	char* selfIpAddress;
	uint16_t port;
	uint16_t model_port;
	uint16_t listen_port; 
    bool* listening_port;
}IP_repo;

void* piMsgReceive(void* ip_repo);

#endif
