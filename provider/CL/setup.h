#ifndef CL_Setup_hpp
#define CL_Setup_hpp

#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "puzzler/core/log.hpp"

void setup(puzzler::ILog *log);
void showPlatforms(std::vector<cl::Platform> * platforms );
void showDevices(std::vector<cl::Device> * devices);
cl::Platform  selectPlatform(std::vector<cl::Platform> * platforms );
cl::Device selectDevice(std::vector<cl::Device> * devices);
std::string LoadSource(const char *fileName);

#endif
