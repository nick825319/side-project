#include "pi_msg_receiver.h"
#include "Socket.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <string.h>
#include <pthread.h>
extern int g_detecting_person;
extern pthread_mutex_t mute_pi_person;

void piMsgReceive_writeRespondTime(std::string content){
    std::ofstream file;
    file.open("measure_pi_composer_Jetson_repondTime.txt",std::ofstream::out | std::ofstream::binary | std::ofstream::app);
    
    std::string writed_string;
    writed_string.append(content + "\n");

	file.write(writed_string.data(), writed_string.length());
    file.close();
}
void piMsgReceive_writeTransferLabelTime(std::string content){
    std::ofstream file;
    file.open("measure_composer_Jetson_TransferLabelTime.txt",std::ofstream::out | std::ofstream::binary | std::ofstream::app);
    
    std::string writed_string;
    writed_string.append(content + "\n");

	file.write(writed_string.data(), writed_string.length());
    file.close();
}

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
        int receivesize;
        while(1){
            signal(SIGPIPE, SIG_IGN);
            memset(buff, 0, sizeof(buff));
            receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
           // std::cout << "receive size: " << receivesize << " ";
            std::string tmp(buff, buff+receivesize);
            if(receivesize <= 0 ){
                delete socket;
                break;
            }
          //  std::cout << "tmp: " << tmp << "\n";
            double composer_sending_time = std::stod(tmp);

            memset(buff, 0, sizeof(buff));
            receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
            std::string tmp2(buff, buff+16);
           // std::cout << "tmp2: " << tmp2 << "\n";
            double pi_composer_respond_time = std::stod(tmp2);
            
            std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(timeNow.time_since_epoch());
            double j_time = (double)ms.count()/(double)1000000;
           // printf("composer_sending_time: %.8f \n", composer_sending_time);
           // printf("j_time: %.8f \n", j_time);
            // !!!note: label_transfer_time include network transfer and c++ inline code process time
            double label_transfer_time = j_time-composer_sending_time;
            
            /*
            printf("composer_sending_time: %.6f \n", composer_sending_time);
            printf("J time: %.6f \n", j_time);
            */
            double FP_respondTime = pi_composer_respond_time + label_transfer_time;
           // printf("full path respond time: %.6f \n", FP_respondTime);
            //printf("label transfer time: %.6f \n", label_transfer_time);
            
            piMsgReceive_writeRespondTime(std::to_string(FP_respondTime));
            piMsgReceive_writeTransferLabelTime(std::to_string(label_transfer_time));
           // !!!note: cus socket send from composer label and time stick together, not socket->Recieve again

            //memset(buff, 0, sizeof(buff));
            //receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);

		   // !!!note: conut pointer address buff pointer + size

            std::string receviceLabel(buff+16, buff+receivesize);
            std::size_t found = receviceLabel.find("person", 0);
            //critical section
            pthread_mutex_lock(&mute_pi_person);   
            if(found != std::string::npos){
                //std::cout << "label : " << receviceLabel << std::endl;
                g_detecting_person = 1 ;
            }else{
                g_detecting_person = 0 ;
            }
            pthread_mutex_unlock(&mute_pi_person);
            //

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

