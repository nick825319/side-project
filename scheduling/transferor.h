#ifndef TRANSF_IMAGE_H
#define TRANSF_IMAGE_H
//#include <cstdint>
#include <string>
#include "videoSource.h"
#include "videoOutput.h" 
#include "Socket.h"

typedef struct{
    char* ipAddress;
    uint16_t listen_jpg_stream_port; 
    videoSource* input;
    videoOutput* output;
}Video_IP_resouce;

std::string get_jpg_bytesStr(int x, int y, int comp, const void*data, int quality);
void send_jpgStream(Socket* socket, std::string str_encode, double time_cam_interval, uint32_t ip, uint16_t port);
void trans_imgStream(void* video_IP_resouce);
int transfer_image(const char* imgPath, char* ipAddress , uint16_t port);
int request_model(char* ipAddress, uint16_t port, char* saved_modelName);
int exist_new_model(char* ipAddress, uint16_t port);
std::string get_file_md5(std::string filename);
std::string create_image_filename();

#endif
