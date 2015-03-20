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

void setup(puzzler::ILog *log, openCLsetupData *, const std::string& kernelName);
void showPlatforms(puzzler::ILog *log, std::vector<cl::Platform> * platforms );
void showDevices(puzzler::ILog *log, std::vector<cl::Device> * devices);
cl::Platform  selectPlatform(puzzler::ILog *log, std::vector<cl::Platform> * platforms );
cl::Device selectDevice(puzzler::ILog *log, std::vector<cl::Device> * devices, uint8_t *);
std::string LoadSource(const std::string& fileName);
void buildProgram(puzzler::ILog *log, cl::Program * program, const std::vector<cl::Device>& devices);

#endif
