#include "signal_handle.h"
#include "transferor.h"
#include "imageIO.h"
#include <iostream>
//#include "cudaMappedMemory.h"
#include <fstream>
#include <cstdlib>
#include <memory>
#include <string>

#include "socket_connect.h"
#include "Socket.h"

#include <openssl/md5.h>
int transfer_image(const char* imgPath, char* ipAddress, uint16_t port){
	/*
	 * load input images
	 */

	/*float* imgCPU = NULL;
	float* imgCUDA = NULL;
	int imgWidth = 0;
	int imgHeight = 0;

	if(!loadImageRGBA(imgPath, (float4**)&imgCPU, (float4**)&imgCUDA, &imgWidth, &imgHeight)){
		printf("transf_image : failed to load image : %s", imgPath);
		return 0;
	}*/

	/*
	 *  read file fstream
	 */
	std::string filename = create_image_filename();
	//std::string filename = std::to_string(imgPath);

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in| std::ifstream::out| std::ifstream::binary);

	if(ifs){
		std::cout<<"read image successfully...:" + filename<< std::endl;	
	}else{
		std::cout<<"read image failed..."<< std::endl;	
	}

	//get length
	ifs.seekg(0, ifs.end);
	int length = ifs.tellg();
	ifs.seekg(0, ifs.beg);
	std::cout<<"length : " + std::to_string(length)<< std::endl;	

	char* buffer = new char [length];
	ifs.read(buffer, length);
	
	
	// create socket
	/*
	* using jetson-utils network Socket.h 
	*/
	
	SocketType TCP_type =  SOCKET_TCP;	

	Socket* socket = socket->Create(SOCKET_TCP);
	std::cout<<"socket Created"<< std::endl;
	socket->Connect(ipAddress, port);
	
	std::string mes_buffer = "SIZE " ;
	mes_buffer = mes_buffer + std::to_string(length);
	char* tmpstring = const_cast<char*>(mes_buffer.c_str());

	/*
	* check byte code in sting
	*/
	/*
	char str_buffer [17];
	str_buffer[16] = 0;
	for(int j = 0; j < mes_buffer.length(); j++){
    		sprintf(&str_buffer[2*j], "%02X", mes_buffer[j]);}
	std::cout << str_buffer << std::endl;
	*/

	// dont use std::string send by socket send, cus encode way and padding will cause error
	
	socket->Send(tmpstring, mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
	std::cout << tmpstring << std::endl;

	// send image
	int rescive_buffersize = 1024;
	uint8_t* buff =  new uint8_t[rescive_buffersize];
	socket->Recieve(buff, rescive_buffersize, NULL, NULL);
	std::string tmp(buff, buff+rescive_buffersize);
	if(tmp.find("GOT SIZE") != std::string::npos){
		std::cout << "server got image size" << std::endl;
		socket->Send(buffer, length, static_cast<uint32_t>(std::stoul(ipAddress)), port);
		std::string text = "FINISH";
		while(true)
		{
			socket->Recieve(buff, rescive_buffersize, NULL, NULL);
			std::string tmp(buff, buff+rescive_buffersize);
			if(tmp.find(text) != std::string::npos){
				break;		
			}
			bzero(buff,sizeof(buff));
			bzero(&tmp,sizeof(tmp));
		}
		std::cout << "transfer image done" << std::endl;
	}
	else{
	std::cout << "transfer image error disconnect" << std::endl;
	
		std::string bye = "BYE";
		char* byestring = const_cast<char*>(bye.c_str());
		socket->Send(byestring, sizeof(byestring)-1, static_cast<uint32_t>(std::stoul(ipAddress)), port);
		delete socket;
	
	}
		
	/*
	* using socket_connect
	*/
	/*
	socket_Connect(ipAddress, port, length);
	*/
				
}
int request_model(char* ipAddress, uint16_t port, char* saved_modelName){
	SocketType TCP_type =  SOCKET_TCP;	

	Socket* socket = socket->Create(SOCKET_TCP);
	std::cout<<"socket Created"<< std::endl;
	socket->Connect(ipAddress, port);
	
	std::string mes_buffer = "MODEL" ;
	
	socket->Send(const_cast<char*>(mes_buffer.c_str()), mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);

	std::cout<<"request new model"<< std::endl;

	int rescive_buffersize = 1024;
	uint8_t* buff =  new uint8_t[rescive_buffersize];
	socket->Recieve(buff, rescive_buffersize, NULL, NULL);
	std::string tmp(buff, buff+rescive_buffersize);
	
	try{
		long model_size;
		size_t model_size_remain;
		if(tmp.find("SIZE") != std::string::npos){
			std::string length = tmp.substr(5);
			// long to size_t, long maybe larger than size_t so it could work
			model_size =std::stol(length);
			model_size_remain = model_size;

			std::cout<<"model_size: ";
			std::cout<< model_size << std::endl;
			
			mes_buffer = "GOT SIZE";
			socket->Send(const_cast<char*>(mes_buffer.c_str()), mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);

			std::ofstream file;
			std::string filename(saved_modelName);
			file.open(filename,std::ofstream::out | std::ofstream::binary);
			std::cout<<"writing file ..." << std::endl;
			uint8_t* buff =  new uint8_t[rescive_buffersize];
			while(model_size_remain >= 0)
			{
				if(model_size_remain == 0)
					break;
				size_t reciveDate = socket->Recieve(buff, rescive_buffersize, NULL, NULL);
				file.write((char*)buff, reciveDate);
				
				model_size_remain -= reciveDate;
				//std::cout<< model_size_remain << " > ";
			}
			file.close();
			delete buff;
			std::cout<<"finish writing file" << std::endl;
			mes_buffer = "BYE";
			socket->Send(const_cast<char*>(mes_buffer.c_str()), mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
		}
	}catch(const std::exception& e){
		std::cout << std::endl;
		std::cout << e.what() << std::endl;
		
	}
	return 0;
}
int exist_new_model(char* ipAddress, uint16_t port){
	SocketType TCP_type =  SOCKET_TCP;	

	Socket* socket = socket->Create(SOCKET_TCP);
	socket->Connect(ipAddress, port);
	
	std::string mes_buffer = "isnew_model" ;
	
	socket->Send(const_cast<char*>(mes_buffer.c_str()), mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);

	std::cout<<"checking new model exist"<< std::endl;

	std::string modelmd5 = get_file_md5("resnet18.onnx");
	socket->Send(const_cast<char*>(modelmd5.c_str()), modelmd5.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);

	int rescive_buffersize = 1024;
	uint8_t* buff =  new uint8_t[rescive_buffersize];
	socket->Recieve(buff, rescive_buffersize, NULL, NULL);
	std::string tmp(buff, buff+rescive_buffersize);

	int result;
	if(tmp.find("NEW MODEL EXIST") != std::string::npos){
		std::cout<<"NEW MODEL EXIST"<< std::endl;
		result = 1;
	}
	else if(tmp.find("NO NEW MODEL") != std::string::npos){
		std::cout<<"NO NEW MODEL"<< std::endl;
		result = 0;
	}

	mes_buffer = "BYE";
	socket->Send(const_cast<char*>(mes_buffer.c_str()), mes_buffer.length(), static_cast<uint32_t>(std::stoul(ipAddress)), port);
	return result;
}
std::string get_file_md5(std::string filename){
	unsigned char* buffer;
	int length;
	std::ifstream f(filename, std::ifstream::binary);
	if(f){
		f.seekg(0, f.end);
		length = f.tellg();
		f.seekg(0, f.beg);

		buffer = new unsigned char [length];

		f.read((char *)buffer, length);
		f.close();
	}

	unsigned char md[16];

	MD5(buffer, length, md);
	
	char tmp[32]={0};
	std::string md5_string;
	for(int i=0;i<16;i++)
	{
		memset(tmp ,0x00 , sizeof(tmp));
		sprintf(tmp, "%02x", md[i]);
		md5_string += tmp;
	}	
	std::cout << md5_string << std::endl;
	return md5_string;
}
std::string create_image_filename(){
	std::string Path = "./imageCapture/";
	std::string file_create_time = "1588576875376"; 
	std::string filename = Path +"Image-" + file_create_time + ".JPEG";
	return filename;
}


