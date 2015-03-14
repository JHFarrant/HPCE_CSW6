#include <iostream>
//#include <CL/cl.h>
//#include <CL/cl_platform.h>
#include <stdexcept>
#include <cstdio>
#define __CL_ENABLE_EXCEPTIONS
#include "cl.hpp"
#include <memory>
#include <streambuf>

// OpenCL initialisation and application setup
void setup(){
    
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    cl::Platform platform;
    
    // get list of OpenCL platforms
    cl::Platform::get(&platforms);
    
    if(platforms.size()==0)
        throw std::runtime_error("No OpenCL platforms found.");
    
    // show platforms
    showPlatforms(&platforms);
    
    // choose platform
    platform=selectPlatform(&platforms);
    
    // get list of OpenCL compatible devices
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if(devices.size()==0)
        throw std::runtime_error("No opencl devices found.\n");
    
    // show devices
    showDevices(&devices);
    
    
    // create context for kernels and memory buffers
    cl::Context context(devices);
    
    // Load kernel Code
    std::string kernelSource=LoadSource("kernel.cl");
    
    // A vector of (data,length) pairs
    cl::Program::Sources sources;
    
    // push kernel
    sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
    
    // collecting everything into a program
    cl::Program program(context, sources);
    
    try{
        program.build(devices);
    }catch(...){
        for(unsigned i=0;i<devices.size();i++){
            std::cerr<<"Log for device "<<devices[i].getInfo<CL_DEVICE_NAME>()<<":\n\n";
            std::cerr<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i])<<"\n\n";
        }
        throw;
    }
    
    
}

void showPlatforms(std::vector<cl::Platform> * platforms ){
    
    std::cerr<<"Found "<<platforms->size()<<" platforms\n";
    for(unsigned i=0;i<platforms->size();i++){
        std::string vendor=platforms->at(i).getInfo<CL_PLATFORM_VENDOR>();
        std::cerr<<"  Platform "<<i<<" : "<<vendor<<"\n";
    }
    
}

void showDevices(std::vector<cl::Device> * devices){
    
    std::cerr<<"Found "<<devices->size()<<" devices\n";
    for(unsigned i=0;i<devices->size();i++){
        std::string name=devices->at(i).getInfo<CL_DEVICE_NAME>();
        std::cerr<<"  Device "<<i<<" : "<<name<<"\n";
    }
}

cl::Platform  selectPlatform(std::vector<cl::Platform> * platforms ){
    int selectedPlatform=0;
    if(getenv("HPCE_SELECT_PLATFORM"))
        selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
    
    std::cerr<<"Choosing platform "<<selectedPlatform<<"\n\n";
    return platforms->at(selectedPlatform);
    
}


cl::Device device selectDevice(std::vector<cl::Device> * devices){
    
    int selectedDevice=0;
    if(getenv("HPCE_SELECT_DEVICE"))
        selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
    
    std::cerr<<"Choosing device "<<selectedDevice<<"\n";
    return devices->at(selectedDevice);
}

// To go in hpce::your_login, just above StepWorldV3OpenCL
std::string LoadSource(const char *fileName)
{
    // Don't forget to change your_login here
    std::string fullName=fileName;
    
    // Open a read-only binary stream over the file
    std::ifstream src(fullName, std::ios::in | std::ios::binary);
    if(!src.is_open())
        throw std::runtime_error("LoadSource : Couldn't load cl file from '"+fullName+"'.");
    
    // Read all characters of the file into a string
    return std::string(
                       (std::istreambuf_iterator<char>(src)), // Node the extra brackets.
                       std::istreambuf_iterator<char>()
                       );
}

