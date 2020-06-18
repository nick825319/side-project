#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>

int socket_Connect(char* ipAddress,uint16_t port, int length){

     //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr(ipAddress);
    info.sin_port = htons(port);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }


    //Send a message to server
    std::string message = "SIZE ";
    message = message + std::to_string(length);

    char str_buffer [17];
	str_buffer[16] = 0;
	for(int j = 0; j < message.length(); j++){
    		sprintf(&str_buffer[2*j], "%02X", message[j]);}
	std::cout << str_buffer << std::endl;
    
    send(sockfd,message.c_str(),sizeof(message.c_str()),0);


    char receiveMessage[100] = {};
    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    printf("%s",receiveMessage);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
	
