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
#include "signal_handle.h"

#include "camera_capture.h"
#include "gstCamera.h"
#include "cudaMappedMemory.h"

#include "imageIO.h"
#include <iostream>
#include <chrono>
#include <unistd.h>

int imageCapture(gstCamera* cam)
{
	gstCamera* camera = cam;

	if( !camera )
	{
		printf("\ncameraCapture:  failed to initialize camera device\n");
		return 0;
	}

	const uint32_t width = camera->GetWidth();
	const uint32_t height = camera->GetHeight();

	printf("\ncameraCapture: successfully open camera device\n");
	printf("    width:  %u\n", width);
	printf("   height:  %u\n", height);

	/*
	 * start streaming
	 */
	if( !camera->Open() )
	{
		printf("cameraCapture:  failed to open camera for streaming\n");
		return 0;
	}
	printf("cameraCapture:  camera open for streaming\n");

	

	/*
	 * capture RGBA image
	 */
	float* imgRGBA = NULL;
	std::chrono::system_clock::time_point second_timepoint = std::chrono::system_clock::now();

	if( !camera->CaptureRGBA(&imgRGBA, 1000, true)){
		printf("cameraCapture:  failed to convert from NV12 to RGBA\n");
	}
	long long time = std::chrono::duration_cast<std::chrono::milliseconds>(second_timepoint.time_since_epoch()).count();
	std::string time_string = std::to_string(time);
	
	/*
	* datetime
	* asctime return formate "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n
	*/
	std::time_t t = std::time(0);   // get time now
	std::string s_datetime = std::asctime(std::localtime(&t));			
	
	std::string outputPath = "imageCapture/";
	std::string filename = "Image-" + time_string + ".JPEG";
	std::cout << "[camera_capture]  captureTime :" << time_string << std::endl;
	std::cout << "[camera_capture]  date:" << s_datetime;
	outputPath = outputPath + filename ;
	int imgWidth = 1280;
	int imgHeight = 720;
	saveImageRGBA(outputPath.c_str(), (float4*)imgRGBA, imgWidth, imgHeight, 255.0f);
	printf("[camera_capture]  saved %s\n", outputPath.c_str());

	/*
	 * destroy resources
	 */
	//printf("cameraCapture:  shutting down...\n");
	//SAFE_DELETE(camera);
	//printf("cameraCapture:  shutdown complete.\n");
	return 0;
}
int period_capture(gstCamera* cam){
	gstCamera* camera = cam;
		if( !camera )
		{
			printf("\ncameraCapture:  failed to initialize camera device\n");
			return 0;
		}

		const uint32_t width = camera->GetWidth();
		const uint32_t height = camera->GetHeight();

		printf("\ncameraCapture: successfully open camera device\n");
		printf("    width:  %u\n", width);
		printf("   height:  %u\n", height);

		/*
		 * start streaming
		 */
		if( !camera->Open() )
		{
			printf("cameraCapture:  failed to open camera for streaming\n");
			return 0;
		}
		printf("cameraCapture:  camera open for streaming\n");

		

		/*
		 * processing loop
		 */
		std::chrono::system_clock::time_point first_timepoint = std::chrono::system_clock::now();
		while(!SIGNAL_RECIEVED)
		{
			// capture RGBA image
			float* imgRGBA = NULL;
			std::chrono::system_clock::time_point second_timepoint = std::chrono::system_clock::now();

			long long  time_interval = std::chrono::duration_cast<std::chrono::milliseconds>(second_timepoint - first_timepoint).count();
			sleep(4);
			//std::cout << "second_timepoint :" << second_timepoint.time_since_epoch().count() << std::endl;
			//std::cout << "time_interval:" << time_interval << std::endl;
			if(time_interval >= 5){
				first_timepoint = second_timepoint;
				std::cout << "[camera_capture] first_timepoint after 5 mili sec:" << first_timepoint.time_since_epoch().count() << std::endl;
				if( !camera->CaptureRGBA(&imgRGBA, 1000, true)){
					printf("cameraCapture:  failed to convert from NV12 to RGBA\n");
				}
				long long time = std::chrono::duration_cast<std::chrono::milliseconds>(second_timepoint.time_since_epoch()).count();
				std::string time_string = std::to_string(time);

				/*
				* datetime
				* asctime return formate "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n
				*/
				std::time_t t = std::time(0);   // get time now
				std::string s_datetime = std::asctime(std::localtime(&t));			

				// saving image
				/*std::ofstream file;
				std::string filename = "Image-" + std::to_string(time) + ".jpeg";
				file.open(filename, ios::binary);	
				file << &imgRGBA ;
				file.close();*/
				
				std::string outputPath = "imageCapture/";
				std::string filename = "Image-" + time_string + ".JPEG";
				std::cout << "[camera_capture]  captureTime :" << time_string << std::endl;
				std::cout << "[camera_capture]  date:" << s_datetime;
				outputPath = outputPath + filename ;
				int imgWidth = 1280;
				int imgHeight = 720;
				saveImageRGBA(outputPath.c_str(), (float4*)imgRGBA, imgWidth, imgHeight, 255.0f);
				printf("[camera_capture]  saved %s\n", outputPath.c_str());
				

				break;
			}
			
			
		}
		/*
		 * destroy resources
		 */
		//printf("cameraCapture:  shutting down...\n");
		//SAFE_DELETE(camera);
		//printf("cameraCapture:  shutdown complete.\n");
	return 0;
}
	



