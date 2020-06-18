#ifndef TRANSF_IMAGE_H
#define TRANSF_IMAGE_H
//#include <cstdint>
#include <string>


int transfer_image(const char* imgPath, char* ipAddress , uint16_t port);
int request_model(char* ipAddress, uint16_t port);
int exist_new_model(char* ipAddress, uint16_t port);
std::string get_file_md5(std::string filename);
std::string create_image_filename();

#endif
