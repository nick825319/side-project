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

#include "imageNet.h"

#include "commandLine.h"
#include "loadImage.h"
#include "cudaFont.h"

#include <string>
#include "string.h"
#include <iostream>
#include <fstream>

imageNet* load_imageNet(char* modelname, char* dataset_path){

	/*
	 * create recognition network
	 */
	char* modelName = modelname;
	char* prototxt = NULL;
	std::string tmp = std::string(dataset_path) + "labels.txt";
	char* labels   = const_cast<char*>(tmp.c_str());
	const char* input    = "input_0";
	const char* output   = "output_0";
	int maxBatchSize = DEFAULT_MAX_BATCH_SIZE;
	
	imageNet* net = imageNet::Create(prototxt, modelName, NULL, labels, input, output, maxBatchSize);

	return net;
}


int classify(char* inputfilename, char* outputfile, imageNet* net)
{
	/*
	 * check input filename
	 */
	char* imgFilename = inputfilename;
	
	if( !net )
	{
		printf("imagenet-console:   failed to initialize imageNet\n");
		return 0;
	}


	float* imgCPU    = NULL;
	float* imgCUDA   = NULL;
	int    imgWidth  = 0;
	int    imgHeight = 0;
		
	if( !loadImageRGBA(imgFilename, (float4**)&imgCPU, (float4**)&imgCUDA, &imgWidth, &imgHeight) )
	{
		printf("failed to load image '%s'\n", imgFilename);
		return 0;
	}
	


	float confidence = 0.0f;
	const int img_class = net->Classify(imgCUDA, imgWidth, imgHeight, &confidence);
	
	// overlay the classification on the image
	if( img_class >= 0 )
	{
		printf("imagenet-console:  '%s' -> %2.5f%% class #%i (%s)\n", imgFilename, confidence * 100.0f, img_class, net->GetClassDesc(img_class));
	
		char* outputFilename = outputfile;
		
		if( outputFilename != NULL )
		{
			// use font to draw the class description
			cudaFont* font = cudaFont::Create(adaptFontSize(imgWidth));
			
			if( font != NULL )
			{
				char str[512];
				sprintf(str, "%2.3f%% %s", confidence * 100.0f, net->GetClassDesc(img_class));

				font->OverlayText((float4*)imgCUDA, imgWidth, imgHeight, (const char*)str, 10, 10,
							   make_float4(255, 255, 255, 255), make_float4(0, 0, 0, 100));
			}

			// wait for GPU to complete work			
			CUDA(cudaDeviceSynchronize());

			// print out performance info
			net->PrintProfilerTimes();

			// save the output image to disk
			printf("imagenet-console:  attempting to save output image to '%s'\n", outputFilename);
			
			if( !saveImageRGBA(outputFilename, (float4*)imgCPU, imgWidth, imgHeight) )
				printf("imagenet-console:  failed to save output image to '%s'\n", outputFilename);
			else
				printf("imagenet-console:  completed saving '%s'\n", outputFilename);
		}
	}
	else
		printf("imagenet-console:  failed to classify '%s'  (result=%i)\n", imgFilename, img_class);
		
	CUDA(cudaFreeHost(imgCPU));
/*
	printf("imagenet-console:  shutting down...\n");

	CUDA(cudaFreeHost(imgCPU));
	SAFE_DELETE(net);

	printf("imagenet-console:  shutdown complete\n");
*/

	return 0;
}

