#ifndef CL_Setup_hpp
#define CL_Setup_hpp

#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include "puzzler/core/log.hpp"

// Courtesy of https://www.khronos.org/registry/cl/specs/opencl-cplusplus-1.1.pdf
// and http://www.cs.bris.ac.uk/home/simonm/montblanc/AdvancedOpenCL_full.pdf


/* A struct containing all the data needed
 by the puzzle programs to execute their kernels */
typedef struct {
    cl::Program * program;
    cl::Context context;
    cl::Device device;
    
} openCLsetupData;

void setup(/*puzzler::ILog *log,*/ openCLsetupData *);
void showPlatforms(std::vector<cl::Platform> * platforms );
void showDevices(std::vector<cl::Device> * devices);
cl::Platform  selectPlatform(std::vector<cl::Platform> * platforms );
cl::Device selectDevice(std::vector<cl::Device> * devices);
std::string LoadSource(const char *fileName);
std::vector<std::pair<cl::Device, unsigned>> createDevicesList(cl::Device device);
cl::Program::Binaries createBinariesList(std::vector<char *> binaries);
//Binaries createBinariesList();

#endif
