#ifndef CAMERA_CAPTURE_H
#define CAMERA_CAPTURE_H
#include "gstCamera.h"

gstCamera* initalize_camera(int d_width= 1280 ,int d_height= 720,char* cameraIndex= "0");
int imageCapture(gstCamera* cam);

void print();

#endif
