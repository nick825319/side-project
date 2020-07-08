#include "signal_handle.h"
#include "pi_msg_receiver.h"

#include "Socket.h"


int piMsgReceive(char* ipAddress, uint16_t port){
	// create socket
	/*
	* using jetson-utils network Socket.h 
	*/
	
	SocketType TCP_type =  SOCKET_TCP;	

	Socket* socket = socket->Create(SOCKET_TCP);
	std::cout<<"socket Created"<< std::endl;
	socket->Connect(ipAddress, port);
	
	// dont use std::string send by socket send, cus encode way and padding will cause error
	
	socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
	std::cout << tmpstring << std::endl;

	int receive_buffersize = 1024;
	uint8_t* buff =  new uint8_t[receive_buffersize];
	socket->Recieve(buff, receive_buffersize, NULL, NULL);
	std::string tmp(buff, buff+receive_buffersize);
	if(tmp.find("PI_DETECT") != std::string::npos){
		std::cout << "receive PI Detect Objects" << std::endl;

		bzero(buff,sizeof(buff));
		socket->Recieve(buff, receive_buffersize, NULL, NULL);

		std::string tmp(buff, buff+receive_buffersize);

		std::cout << tmp << std::endl;
		if(tmp.find("person") != std::string::npos){
			tmp.c		
		}
	
		bzero(buff,sizeof(buff));
		bzero(&tmp,sizeof(tmp));
	}
	else{
	std::cout << "unrecognized token" << std::endl;
		std::string bye = "BYE";
		char* byestring = const_cast<char*>(bye.c_str());
		socket->Send(byestring, sizeof(byestring)-1, static_cast<uint32_t>(std::stoul(ipAddress)), port);
		delete socket;
	
	}
	return 1;			
}


