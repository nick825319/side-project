#include "pi_msg_receiver.h"
#include "Socket.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <string.h>
#include <pthread.h>
extern int g_detecting_person;
extern pthread_mutex_t mute_pi_person;

void* piMsgReceive(void* ip_repo){
    // create socket
    /*
    * using jetson-utils network Socket.h
    */
    int initial = 0;
    uint64_t timeout = 120;
    SocketType TCP_type =  SOCKET_TCP;
    Socket* socket ;  
    while(1){
        socket = socket->Create(SOCKET_TCP);
        socket->Bind(((IP_repo*)ip_repo)->selfIpAddress, ((IP_repo*)ip_repo)->listen_port);
        std::cout<<"waiting.composer..."<< std::endl;
        socket->Accept();
        std::cout<<"socket Created"<< std::endl;

        std::string mes_buffer = "GOTLABEL" ;
        char* tmpstring = const_cast<char*>(mes_buffer.c_str());

        int receive_buffersize = 1024;
        uint8_t* buff =  new uint8_t[receive_buffersize];

        while(1){
            signal(SIGPIPE, SIG_IGN);
            memset(buff, 0, sizeof(buff));
            int receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
            std::cout << "receive size: " << receivesize << " ";
            std::string tmp(buff, buff+receivesize);
            if(receivesize <= 0 ){
                delete socket;
                break;
            }
            double composer_sending_time = std::stod(tmp);
            
            std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
            double j_time = ms.count();
            j_time = j_time / 1000;
            /*
            printf("composer_sending_time: %.6f \n", composer_sending_time);
            printf("J time: %.6f \n", j_time);
            printf("label transfer time: %.6f \n", j_time-composer_sending_time);
            */
            memset(buff, 0, sizeof(buff));
            receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
		    // conut pointer address buff pointer + size
            std::string receviceLabel(buff, buff+receivesize);

            
                /*
                    critical section
                */
                   pthread_mutex_lock(&mute_pi_person);
                   std::size_t found = receviceLabel.find("person", 0);
                   if(found == 0){
                       std::cout << "label : " << receviceLabel << std::endl;
                       g_detecting_person = 1 ;
                   }else{
                       g_detecting_person = 0 ;
                   }
                   pthread_mutex_unlock(&mute_pi_person);
                /*

                */
            if(receviceLabel.find("BYE") != std::string::npos)
		    {
                std::cout << " socket close" << std::endl;
			    break;
		    }
            
		    socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(((IP_repo*)ip_repo)->ipAddress)), ((IP_repo*)ip_repo)->listen_port);

        }
    }

    return NULL;
}

