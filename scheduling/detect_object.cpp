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

#include "gstCamera.h"
#include "glDisplay.h"

#include "signal_handle.h"
/*
* default 
* model_name = ssd-mobilenet-v2
* threshold
*/
detectNet* load_detectNet(char* modelName){
	/*
	 * create detect object network
	 */

	detectNet* net;
	float threshold = 0.5f;
	int maxBatchSize = DEFAULT_MAX_BATCH_SIZE;
	if(strcasecmp(modelName, "ssd-mobilenet-v2") != 0){
		char* prototxt = NULL;
		const char* input    = NULL;
		const char* output   = NULL;
		float meanPixel = 1.0f;
		const char* out_blob     = NULL;
		const char* out_cvg      = NULL;
		const char* out_bbox     = NULL;
		const char* class_labels = NULL;
		/*
		std::string tmp = std::string(dataset_path) + "labels.txt";
		char* labels   = const_cast<char*>(tmp.c_str());
		*/
		net = detectNet::Create(prototxt, modelName, meanPixel, class_labels, threshold, input, 
							out_blob ? NULL : out_cvg, out_blob ? out_blob : out_bbox, maxBatchSize);
	}
	else{
		net = detectNet::Create(detectNet::NetworkTypeFromStr(modelName), threshold, maxBatchSize);
	}
	
	return net;
}


int detect(detectNet* net, gstCamera* camera, glDisplay* display)
{
	/*
	 * create the camera device
	 */
	if( !camera )
	{
		printf("\ndetectnet-camera:  failed to initialize camera device\n");
		return 0;
	}

	/**
	 * Parse a string sequence into OverlayFlags enum.
	 * Valid flags are "none", "box", "label", and "conf" and it is possible to combine flags
	 * (bitwise OR) together with commas or pipe (|) symbol.  For example, the string sequence
	 * "box,label,conf" would return the flags `OVERLAY_BOX|OVERLAY_LABEL|OVERLAY_CONFIDENCE`.
	 */
	char* OverlayFlag = "label";
	const uint32_t overlayFlags = detectNet::OverlayFlagsFromStr(OverlayFlag);

	if( !display ) 
		printf("detectnet-camera:  failed to create openGL display\n");
	if( !camera->Open() )
	{
		printf("detectnet-camera:  failed to open camera for streaming\n");
		return 0;
	}
	
	float confidence = 0.0f;
	
	while( !SIGNAL_RECIEVED )
	{
		// capture RGBA image
		float* imgRGBA = NULL;
		
		if( !camera->CaptureRGBA(&imgRGBA, 1000) )
			printf("detectnet-camera:  failed to capture RGBA image from camera\n");

		// detect objects in the frame
		detectNet::Detection* detections = NULL;
	
		const int numDetections = net->Detect(imgRGBA, camera->GetWidth(), camera->GetHeight(), &detections, overlayFlags);
		
		if( numDetections > 0 )
		{
			printf("%i objects detected\n", numDetections);
		
			for( int n=0; n < numDetections; n++ )
			{
				printf("detected obj %i  class #%u (%s)  confidence=%f\n", n, detections[n].ClassID, net->GetClassDesc(detections[n].ClassID), detections[n].Confidence);
				printf("bounding box %i  (%f, %f)  (%f, %f)  w=%f  h=%f\n", n, detections[n].Left, detections[n].Top, detections[n].Right, detections[n].Bottom, detections[n].Width(), detections[n].Height()); 
			}
		}	

		// update display
		if( display != NULL )
		{
			// render the image
			display->RenderOnce(imgRGBA, camera->GetWidth(), camera->GetHeight());

			// update the status bar
			char str[256];
			sprintf(str, "TensorRT %i.%i.%i | %s | Network %.0f FPS", NV_TENSORRT_MAJOR, NV_TENSORRT_MINOR, NV_TENSORRT_PATCH, precisionTypeToStr(net->GetPrecision()), net->GetNetworkFPS());
			display->SetTitle(str);

			// check if the user quit
			if( display->IsClosed() )
				SIGNAL_RECIEVED = true;
		}

		// print out timing info
		net->PrintProfilerTimes();
	}
	

	/*
	 * destroy resources
	 */
	
	/*printf("detectnet-camera:  shutting down...\n");
	
	SAFE_DELETE(camera);
	SAFE_DELETE(display);
	SAFE_DELETE(net);

	printf("detectnet-camera:  shutdown complete.\n");*/

	return 0;
}

