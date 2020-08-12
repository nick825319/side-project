#include "pi_msg_receiver.h"
#include "Socket.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <string.h>


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

    int receive_buffersize = 1024;
    uint8_t* buff =  new uint8_t[receive_buffersize];

    while(1){
        memset(buff, 0, sizeof(buff));

        /*
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
        std::time_t tt = std::chrono::system_clock::to_time_t(timeNow);
        std::size_t fractional_seconds = ms.count() % 1000;
        std::cout << std::ctime(&tt);
		std::cout << "fraction second:" <<fractional_seconds << std::endl;
        */

        int receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
        std::string tmp(buff, buff+receivesize);
        double composer_sending_time = std::stod(tmp);
        //std::cout << "composer_sending_time(label):" << tmp << std::endl;
        memset(buff, 0, sizeof(buff));

        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
        double j_time = ms.count();
        j_time = j_time / 1000;
        printf("composer_sending_time: %.6f \n", composer_sending_time);
        printf("J time: %.6f \n", j_time);
        printf("label transfer time: %.6f \n", j_time-composer_sending_time);

        receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
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

