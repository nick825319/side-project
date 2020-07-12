#include "signal_handle.h"
#include "pi_msg_receiver.h"

#include "Socket.h"
#include <string>
#include <iostream>
#include <strings.h>


int piMsgReceive(char* ipAddress, uint16_t port){
	// create socket
	/*
	* using jetson-utils network Socket.h 
	*/
	uint64_t timeout = 120;
	SocketType TCP_type =  SOCKET_TCP;	

	Socket* socket = socket->Create(SOCKET_TCP);
	std::cout<<"socket Created"<< std::endl;
	socket->Bind(ipAddress, port);
	socket->Accept();

	std::string mes_buffer = "GOTLABEL" ;
	mes_buffer = mes_buffer + std::to_string(length);
	char* tmpstring = const_cast<char*>(mes_buffer.c_str());

	// dont use std::string send by socket send, cus encode way and padding will cause error
	//socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
	//std::cout << tmpstring << std::endl;

	int receive_buffersize = 1024;
	uint8_t* buff =  new uint8_t[receive_buffersize];

	while(1){
		bzero(buff,sizeof(buff));
		socket->Recieve(buff, receive_buffersize, NULL, NULL);
		std::cout << buff << std::endl;

		socket->Send(buffer, length, static_cast<uint32_t>(std::stoul(ipAddress)), port);
	}
	

	// std::cout << "unrecognized token" << std::endl;
	// 	std::string bye = "BYE";
	// 	char* byestring = const_cast<char*>(bye.c_str());
	// 	socket->Send(byestring, sizeof(byestring)-1, static_cast<uint32_t>(std::stoul(ipAddress)), port);
	// 	delete socket;
	

	return 1;			
}


