#include "delete_imagefile.h"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

#include <chrono>
#include <ctime>

#include <boost/filesystem.hpp>
#include <set>
#include <iterator>
#include <regex>

#define millisecond_month 2629743830L
#define delete_image_period millisecond_month 
#include <typeinfo>
void delete_image(){
	std::time_t t = std::time(0);   // get time now
	//asctime return formate "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n"
	std::string s_datetime = std::asctime(std::localtime(&t));			
	std::string filename = "Image-";
	std::string file_extendsion = ".JPEG";

	std::string path = "imageCapture/";

	filename = "Image--693119329.JPEG";

	std::cout <<"delete path: "<< path <<std::endl;
	//std::cout <<"target file name: "<< filename <<std::endl;
	
	std::cout <<"delete period "<< delete_image_period <<std::endl;
	file_scan_delete(delete_image_period);	
	//int result = unlink((path + filename).c_str());
}
void file_scan_delete(long time_pass){
	std::set<boost::filesystem::path> paths = getDirContents("./imageCapture");
	/*std::copy
		(paths.begin()
		,paths.end()
		,std::ostream_iterator<boost::filesystem::path>(std::cout, "\n")
		);*/
	for(std::set<boost::filesystem::path>::iterator x=paths.begin();x != paths.end();x++)
	{
		std::string fullpath = x->string();
		std::smatch smatch;
		std::ssub_match sb;
		std::regex e ("[0-9]+");

		std::string::size_type sz; //alias of size_t

		long match_ele_millisecond;
		long now_time = getTime_now();

		//std::cout << "now :" << now_time << std::endl;
		//std::cout << "datla :" << now_time - time_pass << std::endl;

		/*
		std::cmatch cmatch;
		std::regex_match(str.c_str() , cmatch , e );
		std::cout << str.c_str();
		for(unsigned i =0; i<cmatch.size(); i++){
			std::cout << cmatch.str(i);
			match_ele_millisecond = std::stol(cmatch.str(i), nullptr, 10);
		}*/	
		std::regex_search(fullpath, smatch, e);
		for(auto match_ele:smatch){
			sb = match_ele;
			std::string st = sb.str();
			if(sb.str().size() > 0){
				match_ele_millisecond = std::stol(st,&sz);
				/*
				* if find that file time has passed for over time_pass(millisseconds) delete it
				*/
				if(now_time - match_ele_millisecond > time_pass)
				{
					//unlink(fullpath.c_str());
					std::cout <<"delete image: : "<< fullpath << std::endl;
				}
			}
		}
	}


}
void delete_model(char* model_name){
	unlink(model_name);
}
std::set<boost::filesystem::path> getDirContents(const std::string& dirname)
{
	std::set<boost::filesystem::path> paths;
	std::copy(boost::filesystem::directory_iterator(dirname)
		 ,boost::filesystem::directory_iterator()
		 ,std::inserter(paths, paths.end())
		 );
	
	return paths;
		
}
long getTime_now(){
	long time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	return time;
}
