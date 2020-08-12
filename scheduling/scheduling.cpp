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

#include "videoSource.h"
#include "videoOutput.h" 

#ifdef HEADLESS
	#define IS_HEADLESS() "headless"	// run without display
#else
	#define IS_HEADLESS() (const char*)NULL
#endif

#define isOpenCam  1
#define isLoadNet  1
#define isOutput_responseTime 0

#define isimageCapture 0
#define istransfer_image 0
#define isrequest_model 0
#define isdelete_image 0
#define isclassify 0
#define isdetect 1
#define ispiMsgReceive 0


bool SIGNAL_RECIEVED = false;
gstCamera* camera;
glDisplay* display;

videoSource* input;
videoOutput* output;

detectNet* net;
imageNet* ImgNet;
int STOPSING = 0;
void signHandler(int dummy){
	STOPSING = 1 ;

	std::time_t t = std::time(0);   // get time now
	std::string s_datetime = std::asctime(std::localtime(&t));
	s_datetime.erase(std::remove(s_datetime.end()-1, s_datetime.end(), '\n'),
               s_datetime.end());
	std::string s = "----stop----";	
	write_responseTime(0, s + s_datetime );

	if(isOpenCam == 1){
    	SAFE_DELETE(input);
		SAFE_DELETE(output);
	}
	if(isLoadNet == 1)
		SAFE_DELETE(net);
	std::cout << "stop process ,release memory" << std::endl;
	exit(0);
}

/*
    legacy code
*/
glDisplay* create_display(){
	display = glDisplay::Create();
	return display;
}
gstCamera* initalize_camera(int d_width,int d_height,char* cameraIndex){
	camera = gstCamera::Create(d_width, d_height, NULL);
	return camera;
}
//

int main( int argc, char** argv )
{
	char* ipAddress = "140.122.185.98";
	char* PiIpAddress = "140.122.184.239";
	char* selfIpAddress = "140.122.184.103";
	uint16_t port = 12000;
	uint16_t model_port = 12100;
	uint16_t piPort = 15200;
	int delta;
	std::chrono::system_clock::time_point startTime;
	std::chrono::system_clock::time_point endTime;

	if(isOpenCam == 1){	
		commandLine cmdLine(argc, argv, IS_HEADLESS());
		input = videoSource::Create(cmdLine, ARG_POSITION(0));
		if( !input )
		{
			LogError("scheduling:  failed to create input stream\n");
			return 0;
		}

		output = videoOutput::Create(cmdLine, ARG_POSITION(1));

		//camera = initalize_camera(500,480);
		//display = create_display();
	}
	if(isLoadNet == 1){
		net = load_detectNet("mb1-ssd.onnx", "./voc-model-label.txt");
		//net = load_detectNet("ssd-mobilenet.onnx", "./labels.txt");
		//net = load_detectNet("ssd-mobilenet-v2", NULL);
	}

	//exist_new_model(ipAddress ,model_port);
	//for(int i=0 ; i<1; i++)
	while(true)
	{
		signal(SIGINT, signHandler);
		if(STOPSING != 1)
		{
			if(isimageCapture == 1){
				startTime = std::chrono::system_clock::now();
				int reslut = imageCapture(camera);
				measure_endTime_peried(startTime, std::string("imageCapture"));
			}
			//	task2-transfer_image
			if(istransfer_image ==1){
				startTime = std::chrono::system_clock::now();
				transfer_image("test",ipAddress ,port);
				measure_endTime_peried(startTime, std::string("transfer_image"));
			}
			//	task3-request_model
			if(isrequest_model==1){
				startTime = std::chrono::system_clock::now();
				request_model(ipAddress, model_port, "mb1-ssd.onnx");
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
			    /*
				test case
			    */
				std::string filename = "Image-1588576875376.JPEG";
				std::string inputfilename = "imageCapture/" + filename;
				std::string outfilename = "classify_result/" + filename;

				startTime = std::chrono::system_clock::now();
				classify(const_cast<char*>(inputfilename.c_str()), const_cast<char*>(outfilename.c_str()), ImgNet);
				measure_endTime_peried(startTime, std::string("classify"));
			}
			//task5-detect object
			if(isdetect ==1){
				detect(net, input, output);
			}
			// task6 - get piCam detection from composer
			if(ispiMsgReceive == 1){
				piMsgReceive(selfIpAddress,  piPort);	
			}				
		}
	}
	std::cout << "exit delta :" << delta << std::endl;
	return 0;
}

void write_responseTime(int delta, std::string outputStr){
	std::ofstream file;
	std::string filename = "task_responseTime.txt";
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
