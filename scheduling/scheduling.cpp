 /*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "scheduling.h"
#include "joystick.hh"

#include "camera_capture.h"
#include "signal_handle.h"
#include "transferor.h"
#include "delete_imagefile.h"

#include "classification.h"
#include "pi_msg_receiver.h"
#include "detect_object.h"

#include <signal.h>
#include <cstring>
#include <string>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stdlib.h>
#include <pthread.h>
#include "videoSource.h"
#include "videoOutput.h" 
#include "pwmCtl.h"

#include <tuple>

#ifdef HEADLESS
	#define IS_HEADLESS() "headless"	// run without display
#else
	#define IS_HEADLESS() (const char*)NULL
#endif

#define isOpenCam  0
#define isOpenDisplay 0
#define isLoadNet 0
#define isOutput_responseTime 0

#define isimageCapture 0
#define istransfer_image 0
#define isrequest_model 0
#define isdelete_image 0
#define isclassify 0
#define isdetect 0
#define ispiMsgReceive 0
#define ismotorCtl 1
#define isimgStream_serv 0

#define isps4Controller 1

#define useWifi 1
bool SIGNAL_RECIEVED = false;


// legacy code
gstCamera* camera;
glDisplay* display;
//
imageNet* ImgNet;
int STOPSING = 0;

Detect_resource* g_detect_resource;
Video_IP_resouce* g_video_IP_resouce;
int g_detecting_person = 0;
std::tuple<float, float> g_objection_center = {0.0, 0.0};
float g_object_width = 0;
float g_object_high = 0;
bool g_server_connected = false;
bool g_useWifi = false;

int g_ps4_forward = 0;
int g_ps4_back = 0;
int g_ps4_left = 0;
int g_ps4_right = 0;
int g_ps4_stop = 0;
int g_ps4_b_left = 0;
int g_ps4_b_down = 0;
int g_ps4_b_right = 0;

pthread_mutex_t mute_pi_person = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mute_objection_center = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mute_ps4_direct_ctl = PTHREAD_MUTEX_INITIALIZER;


void signHandler(int dummy){
	STOPSING = 1 ;
	/*std::time_t t = std::time(0);   // get time now
	std::string s_datetime = std::asctime(std::localtime(&t));
	s_datetime.erase(std::remove(s_datetime.end()-1, s_datetime.end(), '\n'),
               s_datetime.end());
	std::string s = "----stop----";	
	write_responseTime(0, s + s_datetime );*/
}
void release_source(){

    if(isOpenCam == 1){
        std::cout << "release detect_resource->input" << std::endl;
    	SAFE_DELETE(g_detect_resource->input);
	}
    if(isOpenDisplay == 1){
        std::cout << "release detect_resource->output" << std::endl;
        SAFE_DELETE(g_detect_resource->output);
    }
	if(isLoadNet == 1){
        std::cout << "release detect_resource->net" << std::endl;
		SAFE_DELETE(g_detect_resource->net);
    }
    if(ismotorCtl == 1){
        std::cout << "release GPIO" << std::endl;
        unmap_all();
    }
	std::cout << "stop process, release memory" << std::endl;
}
void IP_repo_init(IP_repo* ip_repo){
    ip_repo->ipAddress = "140.122.185.98";
	ip_repo->PiIpAddress = "140.122.184.239";
	ip_repo->selfIpAddress = "140.122.184.103";
    ip_repo->self_WIFI_IpAddress = "192.168.1.100";
	ip_repo->port = 12000;
	ip_repo->model_port = 12100;
	ip_repo->listen_port = 15200;
    ip_repo->listen_jpg_stream_port = 15000;
    ip_repo->listening_port = (bool *)malloc(sizeof(bool));
    *(ip_repo->listening_port) = false;
}
int main( int argc, char** argv )
{
    char* extraargv_0 = "csi://0";
    char* extraargv_1 = "--input-flip=rotate-180";
    int newargc = argc+2;
    char* newargv[newargc];
    for(int i = 0; i< argc;i++){
        newargv[i] = argv[i];
    }
    newargv[argc] = (char*)malloc(strlen(extraargv_0)+1);
    newargv[argc] = extraargv_0;
    newargv[argc+1] = (char*)malloc(strlen(extraargv_1)+1);
    newargv[argc+1] = extraargv_1;
    
    for(int i = 0; i< newargc;i++){
        std::cout << newargv[i] << std::endl;
    }
    IP_repo* ip_repo;
    IP_repo_init(ip_repo);

    g_video_IP_resouce = (Video_IP_resouce *)malloc(sizeof(Video_IP_resouce));
    g_detect_resource = (Detect_resource *)malloc(sizeof(Detect_resource));

	int delta;
	std::chrono::system_clock::time_point startTime;
	std::chrono::system_clock::time_point endTime;

	if(isOpenCam == 1){	
		commandLine cmdLine(newargc, newargv, IS_HEADLESS());
		g_detect_resource->input = videoSource::Create(cmdLine, ARG_POSITION(0));
		if( !g_detect_resource->input )
		{
			LogError("scheduling:  failed to create input stream\n");
			return 0;
		}
	}
    if(isOpenDisplay == 1){	
		commandLine cmdLine(argc, argv, IS_HEADLESS());
		g_detect_resource->output = videoOutput::Create(cmdLine, ARG_POSITION(1));
	}

	if(isLoadNet == 1){
		g_detect_resource->net = load_detectNet("mb1-ssd.onnx", "./voc-model-label.txt");
		//net = load_detectNet("ssd-mobilenet.onnx", "./labels.txt");
		//net = load_detectNet("ssd-mobilenet-v2", NULL);
	}
    if(isimgStream_serv == 1){
        g_video_IP_resouce->ipAddress = ip_repo->ipAddress;
        g_video_IP_resouce->listen_jpg_stream_port = ip_repo->listen_jpg_stream_port;
        g_video_IP_resouce->input = g_detect_resource->input;
        g_video_IP_resouce->output = g_detect_resource->output;
    }
    if(useWifi == 1){
        g_useWifi = true;
    }
    /*
        Thread creat
    */
    pthread_t thr_detect;
    pthread_t thr_piMsg;
    pthread_t thr_motor;
    pthread_t thr_ps4;
    //pthread_create(&thr_detect, NULL, detect, (void *)g_detect_resource);

    // task6 - get piCam detection from composer
	if(ispiMsgReceive == 1){
        pthread_create(&thr_piMsg, NULL, piMsgReceive, (void *)ip_repo);
        //piMsgReceive(ip_repo);
        //pthread_join(thr_piMsg, NULL);
	}
    // task7 - motor control
    if(ismotorCtl == 1){
        //pthread_create(&thr_motor, NULL, pwmctl_forward, NULL);
        //pthread_create(&thr_motor, NULL, presonFollow, NULL);
        //pthread_create(&thr_motor, NULL, reverseRoute_stop, NULL);
        pthread_create(&thr_motor, NULL, control_by_ps4, NULL);

	}
    if(isimgStream_serv == 1){
         //task8 - imgStream to composer
         trans_imgStream(g_video_IP_resouce);
    }
    if(isps4Controller == 1){
         //task9 - blueTooth ps4
         printf("get in ps4\n");
         pthread_create(&thr_ps4, NULL, ps4Controller_run, NULL);
    }        
    //for(int i=0 ; i<1; i++)
	while( !SIGNAL_RECIEVED )
	{
		signal(SIGINT, signHandler);
		if(STOPSING != 1)
		{

            //exist_new_model(ip_repo->ipAddress ,model_port);
			if(isimageCapture == 1){
				startTime = std::chrono::system_clock::now();
				int reslut = imageCapture(camera);
				measure_endTime_peried(startTime, std::string("imageCapture"));
			}
			//	task2-transfer_image
			if(istransfer_image ==1){
				startTime = std::chrono::system_clock::now();
				transfer_image("test",ip_repo->ipAddress ,ip_repo->port);
				measure_endTime_peried(startTime, std::string("transfer_image"));
			}
			//	task3-request_model
			if(isrequest_model==1){
				startTime = std::chrono::system_clock::now();
				request_model(ip_repo->ipAddress, ip_repo->model_port, "mb1-ssd.onnx");
				measure_endTime_peried(startTime, std::string("request_model"));
			}
			//	task3-delete_image
				if(isdelete_image ==1){
				startTime = std::chrono::system_clock::now();
				delete_image();
				measure_endTime_peried(startTime, std::string("delete_image"));
			}
			//	task4-classify
			if(isclassify==1){
                // only test case
				std::string filename = "Image-1588576875376.JPEG";
				std::string inputfilename = "imageCapture/" + filename;
				std::string outfilename = "classify_result/" + filename;

				startTime = std::chrono::system_clock::now();
				classify(const_cast<char*>(inputfilename.c_str()), const_cast<char*>(outfilename.c_str()), ImgNet);
				measure_endTime_peried(startTime, std::string("classify"));
			}
			//task5-detect object
			if(isdetect == 1 ){
                //int a = pthread_create(&thr_detect, NULL, detect, (void *)g_detect_resource);
                //std::cout << "pthread return : " << a  << std::endl;
                //perror("Error: ");
                //pthread_join(thr_detect, NULL);
				detect(g_detect_resource);
                //std::cout << "object center: X:" << std::get<0>(g_objection_center);
                //std::cout << " y:" << std::get<1>(g_objection_center) << std::endl;
			}			
		}
        else{
            break;
        }  
	}
    pthread_join(thr_motor, NULL);
	std::cout << "exit - delta :" << delta << std::endl;
    free(ip_repo->listening_port);
    release_source();
	return 0;
}

void write_responseTime(int delta, std::string outputStr){
	std::ofstream file;
	std::string filename = "measure_task_responseTime.txt";
	file.open(filename,std::ofstream::out | std::ofstream::binary | std::ofstream::app);

    std::string writed_string;
    writed_string.append(outputStr + " : " + std::to_string(delta) + "\n");

	file.write(writed_string.data(), writed_string.length());
	file.close();
}

void measure_endTime_peried(std::chrono::system_clock::time_point startTime, std::string taskname){
	std::chrono::system_clock::time_point endTime;
	int delta;
	
	endTime = std::chrono::system_clock::now();
	delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	std::cout << taskname <<"() response time (milli) : " << delta << std::endl;

	if(isOutput_responseTime == 1){
		write_responseTime(delta, taskname + std::string("() response time"));
	}
}
