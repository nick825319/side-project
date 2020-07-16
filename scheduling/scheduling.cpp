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

#define isOpenCam  0
#define isLoadNet  0

bool SIGNAL_RECIEVED = false;
gstCamera* camera;
glDisplay* display;
detectNet* net;
//imageNet* net
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
    	SAFE_DELETE(camera);
		SAFE_DELETE(display);
	}
	if(isLoadNet == 1)
		SAFE_DELETE(net);
	std::cout << "stop process , safe release memory" << std::endl;
	exit(0);
}

glDisplay* create_display(){
	display = glDisplay::Create();
	return display;
}
gstCamera* initalize_camera(int d_width,int d_height,char* cameraIndex){
	camera = gstCamera::Create(d_width, d_height, cameraIndex);
	return camera;
}


void write_responseTime(int delta, std::string taskname){
	std::ofstream file;
	std::string filename = "task_responseTime.txt";
	file.open(filename,std::ofstream::out | std::ofstream::binary | std::ofstream::app);

    std::string writed_string;
    writed_string.append(taskname);
    writed_string.append(" : ");
    writed_string.append(std::to_string(delta));
	writed_string.append("\n");

	file.write(writed_string.data(), writed_string.length());
	file.close();
}


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
		camera = initalize_camera();
		display = create_display();
	}
	if(isLoadNet == 1){
		net = load_detectNet();
	}

	//exist_new_model(ipAddress ,model_port);
	for(int i=0 ; i<1; i++){
		/*
			task1-imageCapture
			//TODO another trigger imageCapture way 
		*/
		signal(SIGINT, signHandler);
		if(STOPSING != 1)
		{
		/*
			startTime = std::chrono::system_clock::now();

			int reslut = imageCapture(camera);

			endTime = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			std::cout << "imageCapture() response time (milli) : " << delta << std::endl;
			write_responseTime(delta ,std::string("imageCapture() response time"));
			*/

			
			//	task2-transfer_image
			/*
			startTime = std::chrono::system_clock::now();

			transfer_image("test",ipAddress ,port);

			endTime = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			std::cout << "transfer_image() response time (milli) : " << delta << std::endl;
			write_responseTime(delta ,std::string("transfer_image() response time"));
*/
			
			//	task3-request_model
			
			startTime = std::chrono::system_clock::now();

			//request_model(ipAddress, model_port, "mb2-ssd-lite.onnx");

			endTime = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			//std::cout << "request_model() response time (milli) : " << delta << std::endl;
			//write_responseTime(delta ,std::string("request_model() response time"));

			
			//	task3-delete_image
			/*
			startTime = std::chrono::system_clock::now();

			delete_image();

			endTime = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			std::cout << "delete_image() response time (milli) : " << delta << std::endl;
			write_responseTime(delta ,std::string("delete_image() response time"));*/

			
			//	task4-classify
			/*startTime = std::chrono::system_clock::now();
			std::string filename = "Image-1588576875376.JPEG";
			std::string inputfilename = "imageCapture/" + filename;
			std::string outfilename = "classify_result/" + filename;
			
			classify(const_cast<char*>(inputfilename.c_str()), const_cast<char*>(outfilename.c_str()), net);

			endTime = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			std::cout << "classify() response time (milli) : " << delta << std::endl;
			write_responseTime(delta ,std::string("classify() response time"));*/

			//	task5-detect object
			//detect(net, camera, display);

			// task6 - get piCam detection from composer
			piMsgReceive(selfIpAddress,  piPort);	

					
		}
	}
	std::cout << "exit delta :" << delta << std::endl;
	return 0;
}

