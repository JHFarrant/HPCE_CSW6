#include <stdexcept>
#include <memory>
#include <streambuf>
#include "setup.h"
#include <iostream>
#include <fstream>

// OpenCL initialisation and application setup
void setup(/*puzzler::ILog *log,*/ openCLsetupData * setupData){
    
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    cl::Platform platform;
    cl::Device device;
    std::ifstream fp;
    
    // get list of OpenCL platforms
    cl::Platform::get(&platforms);
    
    fprintf(stderr, "-------------------------------------\nOPENCL initialisation\n\n");
    
    if(platforms.size()==0)
        throw std::runtime_error("No OpenCL platforms found.");
    
    // show platforms
    showPlatforms(&platforms);
    
    // select platform
    platform=selectPlatform(&platforms);
    
    // get list of OpenCL compatible devices
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if(devices.size()==0)
        throw std::runtime_error("No opencl devices found.\n");
    
    // show devices
    showDevices(&devices);
    
    // select device
    device=selectDevice(&devices);
    
    // create context for kernels and memory buffers
    cl::Context context(devices);
    
    // Load kernel Code
   /* std::string kernelSource=LoadSource("provider/CL/kernels.cl");
    
    // A vector of (data,length) pairs
    cl::Program::Sources sources;
    
    // push kernel
    sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
    
    // collect all kernel sources into a single program
    cl::Program program(context, sources);*/
    
    // try to load from binary (check status, if unsuccessful, load and compile source)
    
    cl::Program * program = NULL;
    fp.open("provider/CL/kernels.raw", std::ifstream::binary);
    
    // Check if binary already exists
    if(fp.is_open()){
        // get compiled binary from runtime
        cl::Program::Binaries binaries;
        
        /*
        for(int  i =0; i < devices.size(); i++){
            binarise.push_back();
        }*/
        
        //cl::Program program(context, devices, binaries);
        //cl::Program program();
        
    }else{ // Load kernel code and compile it
        
         //Load kernel Code
         std::string kernelSource=LoadSource("/Users/David/Documents/Imperial/4thYear/HPCE/HPCE_CSW6/provider/CL/kernels.cl");
         
         // A vector of (data,length) pairs
         cl::Program::Sources sources;
         
         // push kernel
         sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
         
         // collect all kernel sources into a single program
        program = new cl::Program(context, sources);
        
         std::vector<char *> binaries(devices.size());
         std::vector<size_t> binarySizes(devices.size());
        
         // save to binary format
         //program->getInfo<size_t>(CL_PROGRAM_BINARY_SIZES, &binarySizes[0]);
        
        
         program->getInfo<CL_PROGRAM_BINARY_SIZES>();
        
         //program->getInfo(CL_PROGRAM_BINARIES , &binaries[0]);

        
    }

    
    
   /* cl::Program * program = new cl::Program();
   
    
    //program(context,  devices, );
    
    // Check if binaries already exist
    std::vector<char *> binaries;
    std::vector<size_t> binarySizes;
    
    //cl::Program::Binaries binaries;
    //cl::Platform::getInfo(CL_PROGRAM_BINARIES , binaries);
    
    
    program->getInfo(CL_PROGRAM_BINARY_SIZES, &binarySizes)
    program->getInfo(CL_PROGRAM_BINARIES , &binaries);
    delete program;
    
    // if there are no binaries, load and compile kernel code
    if(binaries.size() == 0){
        
        // Load kernel Code
        std::string kernelSource=LoadSource("provider/CL/kernels.cl");
        
        // A vector of (data,length) pairs
        cl::Program::Sources sources;

        // push kernel
        sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));

        // collect all kernel sources into a single program
        program = new cl::Program(context, sources);
        
        
        
    } else {
        //program = cl::Program(context, devices, binaries);
        
       // cl::Program program(context, devices, binaries);
        
        // Load Binaries
        program = new cl::Program(context, devices, createBinariesList(binaries));


    }*/
    
    
    
    /*cl::Program::Binaries binaries(devices.size());

    // get compiled binary from runtime
    //cl::Program p =  cl::Program(context, devices, binaries);

    cl::Program * program = new cl::Program(context, devices, binaries);
    
    */
    
    try{ // build program
        program->build(devices);
        fprintf(stderr, "-------------------------------------\n");
    }catch(...){
        for(unsigned i=0;i<devices.size();i++){
            
            fprintf(stderr,"Log for device %s \n\n", (devices[i].getInfo<CL_DEVICE_NAME>()).c_str());
            fprintf(stderr,"%s \n\n", program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]).c_str());

        }
        throw;
    }
    
    // copy data
    setupData->program = program;
    setupData->context = context;
    setupData->device = device;
    

}

void showPlatforms(std::vector<cl::Platform> * platforms ){
    
    fprintf(stderr,"Found %d platforms \n", platforms->size());
    
    for(unsigned i=0;i<platforms->size();i++){
        std::string vendor=platforms->at(i).getInfo<CL_PLATFORM_VENDOR>();
        fprintf(stderr,"Platform %d : %s \n", i, vendor.c_str());

    }
    
}

void showDevices(std::vector<cl::Device> * devices){
    
    std::cerr<<"Found "<<devices->size()<<" devices\n";
    for(unsigned i=0;i<devices->size();i++){
        std::string name=devices->at(i).getInfo<CL_DEVICE_NAME>();
        fprintf(stderr,"  Device %d : %s\n", i, name.c_str());
    }
}

cl::Platform selectPlatform(std::vector<cl::Platform> * platforms ){
    int selectedPlatform=0;
    if(getenv("HPCE_SELECT_PLATFORM"))
        selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
    
    fprintf(stderr,"Choosing platform %d \n\n", selectedPlatform);
    return platforms->at(selectedPlatform);
    
}


cl::Device selectDevice(std::vector<cl::Device> * devices){
    
    int selectedDevice=0;
    if(getenv("HPCE_SELECT_DEVICE"))
        selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
    
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


// returns a vector list of Devices with each pair in the form (device, sizeof(device))
std::vector<std::pair<cl::Device, unsigned>> createDevicesList(cl::Device device){

    std::vector<std::pair<cl::Device, unsigned>> devices;
    devices.push_back(std::pair<cl::Device, unsigned>(device, sizeof(device)));
    
    
    // although it's only one device, we'll still return it as a vector of devices
    return devices;
    
}

// Converts the given vector to a vector list of Binaries with each pair in the form
// (binary, sizeof(Binary)). The vector will only contain 1 element which corresponds
// to the previously selected Device
cl::Program::Binaries createBinariesList(std::vector<char *> binaries){
    
    
    cl::Program::Binaries binariesList;
    
    int selectedDevice=0;
    if(getenv("HPCE_SELECT_DEVICE"))
        selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));

    // get Device id
    char * b = binaries[selectedDevice];
    
    binariesList.push_back(std::pair<const char *, unsigned>(b, sizeof(b)));
    
    
    // although it's only one device, we'll still return it as a vector of devices
    return binariesList;
    
}

/*

cl::Program::VECTOR_CLASS<Devices&> createBinariesList(){
    
    
    
    
}*/


