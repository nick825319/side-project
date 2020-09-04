#ifndef SCHEDULING_H
#define SCHEDULING_H
#include <iostream>
#include "gstCamera.h"
#include "glDisplay.h"
#include <chrono>
#include "detect_object.h"


void signHandler(int dummy);
void write_responseTime(int delta, std::string outputStr);
void measure_endTime_peried(std::chrono::system_clock::time_point startTime, std::string taskname);
int main( int argc, char** argv );

#endif
	

