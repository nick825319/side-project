#include "pi_msg_receiver.h"
#include "Socket.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <pthread.h>
#include <list>
extern int g_detecting_person;
extern pthread_mutex_t mute_pi_person;

std::list<std::string> decode_packet(std::string content){
    //std::string content = "451236.1648-0.02321616-bird0.23";
    int count = 0;
    std::list<std::string> time_list;
    int str_indexCount = 0;
    for(int i = 0; i < content.size(); i++){
        if(content[i] != '-' && i + 1 != content.size()){
            count ++;
        }else{
            if(i + 1 == content.size()){
                count += 1 ;
            }
            time_list.push_back(content.substr(str_indexCount, count));
            count ++ ;
            str_indexCount = i + 1;
            count = 0;
        }
    }/*
    for(auto ele : time_list){
            std::cout << ele << "\n";
        }*/
    return time_list;
}
double  get_time_sec(){
    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(timeNow.time_since_epoch());
    return (double)ms.count()/(double)1000000;
     
}   
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
    std:: list<std::string> recv_timeLabel;
    while(1){
        socket = socket->Create(SOCKET_TCP);
        socket->Bind(((IP_repo*)ip_repo)->selfIpAddress, ((IP_repo*)ip_repo)->listen_port);
        std::cout<<"waiting.composer..."<< std::endl;
        socket->Accept();
        std::cout<<"socket Created"<< std::endl;

        int receive_buffersize = 1024;
        uint8_t* buff =  new uint8_t[receive_buffersize];
        int receivesize;
        while(1){
            signal(SIGPIPE, SIG_IGN);
            memset(buff, 0, sizeof(buff));

            receivesize = socket->Recieve(buff, receive_buffersize, NULL, NULL);
            std::string receive_content(buff, buff+receivesize);
            if(receivesize == 0){
                break;            
            }
            double time_afterRecv = get_time_sec();

            recv_timeLabel = decode_packet(receive_content);

            double composer_sending_time = std::stod(recv_timeLabel.front());
            recv_timeLabel.pop_front();
            double pi_composer_respond_time = std::stod(recv_timeLabel.front());
            recv_timeLabel.pop_front();
            std::string receviceLabel = recv_timeLabel.front();

           // printf("composer_sending_time: %.8f \n", composer_sending_time);
           // printf("time_afterRecv: %.8f \n", time_afterRecv);

            double label_transfer_time = time_afterRecv-composer_sending_time;
            
           // double FP_respondTime = pi_composer_respond_time + label_transfer_time;
           // printf("full path respond time: %.6f \n", FP_respondTime);
           // printf("label transfer time: %.6f \n", label_transfer_time);
            
           // piMsgReceive_writeRespondTime(std::to_string(FP_respondTime));
           // piMsgReceive_writeTransferLabelTime(std::to_string(label_transfer_time));

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

            std::string mes_buffer = "GOTLABEL" ;
            char* tmpstring = const_cast<char*>(mes_buffer.c_str());
		    socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(((IP_repo*)ip_repo)->ipAddress)), ((IP_repo*)ip_repo)->listen_port);

        }
    }

    return NULL;
}

