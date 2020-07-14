#include "pi_msg_receiver.h"
#include "Socket.h"
#include <string>
#include <iostream>
#include <strings.h>
#include <chrono>
#include <ctime>

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
    char* tmpstring = const_cast<char*>(mes_buffer.c_str());

    // dont use std::string send by socket send, cus encode way and padding will cause error
    //socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
    //std::cout << tmpstring << std::endl;

    int receive_buffersize = 1024;
    uint8_t* buff =  new uint8_t[receive_buffersize];

    while(1){
        bzero(buff,sizeof(buff));

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
        std::time_t tt = std::chrono::system_clock::to_time_t(timeNow);
        std::size_t fractional_seconds = ms.count() % 1000;
        std::cout << std::ctime(&tt);
		std::cout << "fraction second:" <<fractional_seconds << std::endl;

        int receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
		// conut pointer address buff pointer + size
        std::string receviceLabel(buff, buff+receivesize);
		if(receviceLabel.find("BYE") != std::string::npos)
		{
			delete socket;
			break;
		}
        std::cout << receviceLabel << std::endl;
		

		socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);


		std::cout << std::endl;
    }

    return 1;
}

