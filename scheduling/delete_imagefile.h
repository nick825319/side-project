#ifndef DELETE_IMAGEFILE_H_
#define DELETE_IMAGEFILE_H_

#include <boost/filesystem.hpp>
#include <set>

void delete_image();
void file_scan_delete(long time_pass);
std::set<boost::filesystem::path> getDirContents(const std::string& dirname);
void delete_model(char* model_name = "model.onnx");
long getTime_now();

#endif
