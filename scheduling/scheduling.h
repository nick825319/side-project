#ifndef SCHEDULING_H
#define SCHEDULING_H
#include <iostream>
#include "gstCamera.h"
#include "glDisplay.h"

gstCamera* initalize_camera(int d_width= 640 ,int d_height= 480,char* cameraIndex= "0");
glDisplay* create_display();

void signHandler(int dummy);
void write_responseTime(int delta, std::string taskname);
int main( int argc, char** argv );

#endif
	

