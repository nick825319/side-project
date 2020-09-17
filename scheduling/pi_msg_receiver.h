#ifndef PI_MSG_RECEIVER_H
#define PI_MSG_RECEIVER_H
#include <string>
#include "scheduling.h"

typedef struct{
    char* ipAddress;
	char* PiIpAddress;
    char* self_WIFI_IpAddress;
	char* selfIpAddress;
	uint16_t port;
	uint16_t model_port;
	uint16_t listen_port;
    uint16_t listen_jpg_stream_port; 
    bool* listening_port; 
}IP_repo;

void* piMsgReceive(void* ip_repo);

#endif
