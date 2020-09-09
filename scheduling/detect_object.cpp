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

#include "detectNet.h"
#include "detect_object.h"
#include "scheduling.h"

#include "videoSource.h"
#include "videoOutput.h" 

#include "signal_handle.h"
#include <chrono>
#include <iostream>
#include <unistd.h>

#include <tuple>
#include <pthread.h>
/*
* default 
* model_name = ssd-mobilenet-v2
* threshold
*/
extern std::tuple<float, float> g_objection_center;
extern pthread_mutex_t mute_objection_center;
extern float g_object_width;
extern float g_object_high;

detectNet* load_detectNet(char* modelName, char* dataset_path){
	/*
	 * create detect object network
	 */

	detectNet* net;
	float threshold = 0.5f;
	int maxBatchSize = DEFAULT_MAX_BATCH_SIZE;
	if(strcasecmp(modelName, "ssd-mobilenet-v2") != 0){
		char* prototxt = NULL;
		const char* input    = "input_0";
		const char* output   = NULL;
		float meanPixel = 1.0f;
		const char* out_blob     = NULL;
		const char* out_cvg      = "scores";
		const char* out_bbox     = "boxes";
		
		std::string tmp = std::string(dataset_path);
		const char* class_labels   = const_cast<char*>(tmp.c_str());
		
		net = detectNet::Create(prototxt, modelName, meanPixel, class_labels, threshold, input, 
							out_blob ? NULL : out_cvg, out_blob ? out_blob : out_bbox, maxBatchSize);
	}
	else{
		net = detectNet::Create(detectNet::NetworkTypeFromStr(modelName), threshold, maxBatchSize);
	}
	
	return net;
}


void* detect(void* detect_resource)
{
	/*
	 * create the camera device
	 */
	if( !((Detect_resource *)detect_resource)->input )
	{
		printf("\ndetectnet-camera:  failed to initialize camera device\n");
		return NULL;
	}
	if( !((Detect_resource *)detect_resource)->output )
		LogError("detectnet:  failed to create output stream\n");
    
	/**
	 * Parse a string sequence into OverlayFlags enum.
	 * Valid flags are "none", "box", "label", and "conf" and it is possible to combine flags
	 * (bitwise OR) together with commas or pipe (|) symbol.  For example, the string sequence
	 * "box,label,conf" would return the flags `OVERLAY_BOX|OVERLAY_LABEL|OVERLAY_CONFIDENCE`.
	 */
	char* OverlayFlag = "box,labels,conf";
	const uint32_t overlayFlags = detectNet::OverlayFlagsFromStr(OverlayFlag);
	
	float confidence = 0.0f;
	
	
	// capture RGBA image
	uchar3* imgRGBA = NULL;
	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
	if( !((Detect_resource *)detect_resource)->input->Capture(&imgRGBA, 1000) )
		printf("detectnet-camera:  failed to capture RGBA image from camera\n");
    


	// detect objects in the frame
	detectNet::Detection* detections = NULL;

    std::chrono::system_clock::time_point inference_startTime = std::chrono::system_clock::now();

	const int numDetections = ((Detect_resource *)detect_resource)->net->Detect(imgRGBA, ((Detect_resource *)detect_resource)->input->GetWidth(), ((Detect_resource *)detect_resource)->input->GetHeight(), &detections, overlayFlags);

	std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
	int delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - inference_startTime).count();
    int responsed_delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	std::cout <<"inference time (milli) : " << delta << std::endl;
    std::cout <<"detect object responsed_time (milli) : " << delta << std::endl;

    pthread_mutex_lock(&mute_objection_center);
    std::get<0>(g_objection_center) = 0;
    std::get<1>(g_objection_center) = 0;
    g_object_width = 0;
    g_object_high = 0;
    pthread_mutex_unlock(&mute_objection_center);

	if( numDetections > 0 )
	{
		printf("%i objects detected\n", numDetections);
	
		for( int n=0; n < numDetections; n++ )
		{
			printf("detected obj %i  class #%u (%s)  confidence=%f\n", n, detections[n].ClassID, ((Detect_resource *)detect_resource)->net->GetClassDesc(detections[n].ClassID), detections[n].Confidence);
			printf("bounding box %i  (%f, %f)  (%f, %f)  w=%f  h=%f\n", n, detections[n].Left, detections[n].Top, detections[n].Right, detections[n].Bottom, detections[n].Width(), detections[n].Height()); 

            if(detections[n].ClassID == 15){
                pthread_mutex_lock(&mute_objection_center);
                g_object_width = detections[n].Right - detections[n].Left;
                g_object_high = detections[n].Bottom - detections[n].Top;
                std::get<0>(g_objection_center) = (detections[n].Right - detections[n].Left)/2 + detections[n].Left;
                std::get<1>(g_objection_center) = (detections[n].Bottom - detections[n].Top)/2 + detections[n].Top;
                pthread_mutex_unlock(&mute_objection_center);
            }
            
		}
	}	

	// update display
	if( ((Detect_resource *)detect_resource)->output != NULL )
	{
		// render the image
		((Detect_resource *)detect_resource)->output->Render(imgRGBA, ((Detect_resource *)detect_resource)->input->GetWidth(), ((Detect_resource *)detect_resource)->input->GetHeight());

		// update the status bar
		char str[256];
		sprintf(str, "TensorRT %i.%i.%i | %s | Network %.0f FPS", NV_TENSORRT_MAJOR, NV_TENSORRT_MINOR, NV_TENSORRT_PATCH, precisionTypeToStr(((Detect_resource *)detect_resource)->net->GetPrecision()), ((Detect_resource *)detect_resource)->net->GetNetworkFPS());
		((Detect_resource *)detect_resource)->output->SetStatus(str);

		// check if the user quit
		if( !((Detect_resource *)detect_resource)->output->IsStreaming() )
			SIGNAL_RECIEVED = true;
	}

	// print out timing info
	((Detect_resource *)detect_resource)->net->PrintProfilerTimes();
	usleep(100);
	return NULL;
}

