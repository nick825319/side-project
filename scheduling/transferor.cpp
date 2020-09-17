#include "signal_handle.h"
#include "transferor.h"
#include "imageIO.h"
#include "imageFormat.h"
#include <iostream>
#include <fstream>
#include "socket_connect.h"
#include "Socket.h"
#include <unistd.h>
#include "detect_object.h"

#include "cudaNormalize.h"

//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <openssl/md5.h>
extern bool SIGNAL_RECIEVED;
extern bool g_server_connected;
void transferor_writeCameraIntervalTime(std::string content){
    std::ofstream file;
    file.open("measure_CameraInterval.txt",std::ofstream::out | std::ofstream::binary | std::ofstream::app);
    
    std::string writed_string;
    writed_string.append(content + "\n");

	file.write(writed_string.data(), writed_string.length());
    file.close();
}
void transferor_writeCV_JPGTime(std::string content){
    std::ofstream file;
    file.open("measure_CV_JPG.txt",std::ofstream::out | std::ofstream::binary | std::ofstream::app);
    
    std::string writed_string;
    writed_string.append(content + "\n");

	file.write(writed_string.data(), writed_string.length());
    file.close();
}
double  transferor_get_time_sec(){
    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(timeNow.time_since_epoch());
    return (double)ms.count()/(double)1000000;
     
}  
void trans_imgStream(void* video_IP_resouce){
    const int width = 1280;
    const int height = 720;
    const int channels = 3;
    const int quality = 90;
    double time_start_cam;
    double time_end_cam;
    double time_cam_interval;
    uint32_t uint32_ip = static_cast<uint32_t>(std::stoul(((Video_IP_resouce*)video_IP_resouce)->ipAddress));
    uint16_t port = ((Video_IP_resouce*)video_IP_resouce)->listen_jpg_stream_port;

    bool connected = false;
    //initial socket
    SocketType TCP_type =  SOCKET_TCP;	
	Socket* socket = socket->Create(SOCKET_TCP);
	std::cout<<"socket Created..."<< std::endl;

    while(!connected){
        if(g_server_connected != true){
            continue;
        }
        std::cout<<"socket trying to connect... 98 -p 15000"<< std::endl;
	    connected = socket->Connect(((Video_IP_resouce*)video_IP_resouce)->ipAddress, 
                            ((Video_IP_resouce*)video_IP_resouce)->listen_jpg_stream_port); 

        while( !SIGNAL_RECIEVED )
	    {
            if(!SIGNAL_RECIEVED){
                if( !((Video_IP_resouce *)video_IP_resouce)->input )
	            {
		            printf("\ndetectnet-camera:  failed to initialize camera device\n");
		            return;
	            }

                // capture RGB image
	            uchar3* imgRGB = NULL;
                
                time_start_cam = transferor_get_time_sec();
                // Capture(T** image, uint64 timeout=UINT64_MAX) -
	            if( !((Video_IP_resouce *)video_IP_resouce)->input->Capture(&imgRGB, 1000) ){
		            printf("detectnet-camera:  failed to capture RGB image from camera\n");
                }
                time_end_cam = transferor_get_time_sec();
                time_cam_interval =  time_end_cam - time_start_cam;
                //transferor_writeCameraIntervalTime(std::to_string(time_cam_interval));
                //saveImage("capture.jpg", imgRGB, 1280, 720);
                if( ((Video_IP_resouce *)video_IP_resouce)->output != NULL && SIGNAL_RECIEVED != true )
	            {
		            ((Video_IP_resouce *)video_IP_resouce)->output->Render(imgRGB, 
                                                                        ((Video_IP_resouce *)video_IP_resouce)->input->GetWidth(), 
                                                                        ((Video_IP_resouce *)video_IP_resouce)->input->GetHeight());
                    if( !((Video_IP_resouce *)video_IP_resouce)->output->IsStreaming() ){
			                SIGNAL_RECIEVED = true;
                    }
	            }
                
               /* CUDA(cudaNormalize((float4*)imgRGB, make_float2(0,255),
                                       (float4*)imgRGB, make_float2(0,1),
                                        width, height));*/
              //  CUDA(cudaDeviceSynchronize());
                if(imgRGB != NULL){
                    double time_start_CV = transferor_get_time_sec();

                    cv::Mat cv_image(cv::Size(width, height), CV_8UC3);
                    cv::Mat cv_image2(cv::Size(width, height), CV_8UC3, imgRGB);
                    cv::cvtColor(cv_image2, cv_image, cv::COLOR_RGB2BGR);
                    
                    //save jpg as file ,named with time
                   /* std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
                    long long time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();
                    std::string time_string = std::to_string(time);
                    std::string filename = "Image-" + time_string + ".jpg";
                    cv::imwrite(".jpg", cv_image);*/

                    std::vector<uchar> encodedjpg;
                    cv::imencode(".jpg", cv_image, encodedjpg);
                    std::string str_encode(encodedjpg.begin(), encodedjpg.end());

                    double time_end_CV = transferor_get_time_sec();
                    double time_CV_interval =  time_end_CV - time_start_CV;
                    transferor_writeCV_JPGTime(std::to_string(time_CV_interval));

                    send_jpgStream(socket, str_encode,time_cam_interval, uint32_ip, port);
                    /*
	                * check byte code in sting
	                *//*
                    std::string str_buffer;
	                for(int j = 0; j < str_encode.length(); j++){
                            char code_buffer [10];
                            sprintf(&code_buffer[0], "0x%02X ", str_encode[j]);
                            std::string strbuff(code_buffer);
                    		str_buffer.append(strbuff);
                    }
	                std::cout << str_buffer << std::endl;*/
                }
            }
        } 
    } 
    return;
}

void send_jpgStream(Socket* socket, std::string str_encode, double time_cam_interval, uint32_t ip, uint16_t port){
     int imageSize = str_encode.length();
     double send_time = transferor_get_time_sec(); 
     std::string timestamp_pack;

     timestamp_pack.append(std::to_string(send_time));
     timestamp_pack.append("-");
     timestamp_pack.append(std::to_string(time_cam_interval));
     timestamp_pack.append("-");
     timestamp_pack.append(std::to_string(imageSize));
     timestamp_pack.append("\n");
     //std::cout << timestamp_pack << std::endl;
     
	 char* tmp_charArray = const_cast<char*>(timestamp_pack.c_str());
     socket->Send(tmp_charArray, timestamp_pack.length(), ip, port);


     tmp_charArray = const_cast<char*>(str_encode.c_str());
     socket->Send(tmp_charArray, imageSize, ip, port);

}
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


