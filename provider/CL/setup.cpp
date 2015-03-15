#include <stdexcept>
#include <memory>
#include <streambuf>
#include "setup.h"
#include <iostream>
#include <fstream>

// OpenCL initialisation and application setup
void setup(puzzler::ILog *log){
    
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    cl::Platform platform;
    
    // get list of OpenCL platforms
    cl::Platform::get(&platforms);
    
    fprintf(stderr, "-------------------------------------\nOPENCL initialisation\n\n");
    
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
    std::string kernelSource=LoadSource("provider/kernels.cl");
    
    // A vector of (data,length) pairs
    cl::Program::Sources sources;
    
    // push kernel
    sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
    
    // collect everything into a program
    cl::Program program(context, sources);
    
    try{ // build program
        program.build(devices);
        fprintf(stderr, "-------------------------------------\n");
    }catch(...){
        for(unsigned i=0;i<devices.size();i++){
           // std::cerr<<"Log for device "<<devices[i].getInfo<CL_DEVICE_NAME>()<<":\n\n";
            //fprintf(stderr, "Log for device %s \n\n", (devices[i].getInfo<CL_DEVICE_NAME>()).c_str() );
           //std::cerr<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i])<<"\n\n";
           // fprintf(stderr, "%s \n\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]).c_str() );
            
            fprintf(stderr,"Log for device %s \n\n", (devices[i].getInfo<CL_DEVICE_NAME>()).c_str());
            fprintf(stderr,"%s \n\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]).c_str());

        }
        throw;
    }
    
}

void showPlatforms(std::vector<cl::Platform> * platforms ){
    
    //std::cerr<<"Found "<<platforms->size()<<" platforms\n";
    fprintf(stderr,"Found %d platforms \n", platforms->size());
    
    for(unsigned i=0;i<platforms->size();i++){
        std::string vendor=platforms->at(i).getInfo<CL_PLATFORM_VENDOR>();
        //std::cerr<<"  Platform "<<i<<" : "<<vendor<<"\n";
       fprintf(stderr,"Platform %d : %s \n", i, vendor.c_str());

    }
    
}

void showDevices(std::vector<cl::Device> * devices){
    
    std::cerr<<"Found "<<devices->size()<<" devices\n";
    //fprintf(stderr, "Found %s\n", devices->size().c_str());
    for(unsigned i=0;i<devices->size();i++){
        std::string name=devices->at(i).getInfo<CL_DEVICE_NAME>();
       // std::cerr<<"  Device "<<i<<" : "<<name<<"\n";
        fprintf(stderr,"  Device %d : %s\n", i, name.c_str());
    }
}

cl::Platform  selectPlatform(std::vector<cl::Platform> * platforms ){
    int selectedPlatform=0;
    if(getenv("HPCE_SELECT_PLATFORM"))
        selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
    
    //std::cerr<<"Choosing platform "<<selectedPlatform<<"\n\n";
    fprintf(stderr,"Choosing platform %d \n\n", selectedPlatform);
    return platforms->at(selectedPlatform);
    
}


cl::Device selectDevice(std::vector<cl::Device> * devices){
    
    int selectedDevice=0;
    if(getenv("HPCE_SELECT_DEVICE"))
        selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
    
    //std::cerr<<"Choosing device "<<selectedDevice<<"\n";
    fprintf(stderr,"Choosing device %d \n", selectedDevice);
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

