#include <stdexcept>
#include <memory>
#include <streambuf>
#include "setup.h"
#include <iostream>
#include <fstream>

// OpenCL initialisation and application setup
void setup(puzzler::ILog *log,openCLsetupData * setupData, const std::string& kernelName){
    
    std::vector<cl::Platform> platforms;
    std::vector<cl::Device> devices;
    cl::Platform platform;
    cl::Device device; // device object
    uint8_t devId = 0; // device id
    std::ifstream finp;
    std::ofstream fout;   

 
    // get list of OpenCL platforms
    cl::Platform::get(&platforms);
    
    log->LogInfo( "-------------------------------------\nOPENCL initialisation\n");
    
    if(platforms.size()==0)
        throw std::runtime_error("No OpenCL platforms found.");
    
    // show platforms
    showPlatforms(log, &platforms);
    
    // select platform
    platform=selectPlatform(log, &platforms);
    
    // get list of OpenCL compatible devices
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if(devices.size()==0)
        throw std::runtime_error("No opencl devices found.\n");
    
    // show devices
    showDevices(log, &devices);
    
    // select device
    device=selectDevice(log, &devices, &devId);
    
    // create context for kernels and memory buffers
    cl::Context context(devices);
    
    
    // try to load from binary (check status, if unsuccessful, load and compile source) 
    cl::Program * program = NULL;

    std::string fname = "provider/CL/" + kernelName + ".bin";	
    finp.open(fname, std::ios::in | std::ios::binary);

    // Check if binary already exists
    if(finp.is_open()){


	log->LogInfo( "\nsuccesfully opened binary file\n");

        // get compiled binaries from runtime
	// Note even if we want the binary of only one device,
        // our vector still needs to have devices.size() elements
	// for it to be correctly interpreted
        cl::Program::Binaries binaries(devices.size());
	
   	 // get length of file:
    	finp.seekg (0, finp.end);
    	int length = finp.tellg();
    	finp.seekg (0, finp.beg);


    	// allocate memory for binary of interest
    	char * cb = new char [length];
	log->LogInfo("\nallocating %d bytes\n", length);
	
	// read binary into buffer
	finp.read(cb, length);

	// add to binaries vector
	binaries[devId] = std::pair<const char *, unsigned>(cb, length);

	// close binary file
	finp.close();
	
	log->LogInfo( "\nCopied to binary to  binaries vector\n");

	// Create kernel program by also providing the binaries	
        program = new cl::Program(context, devices, binaries);

	// build program
        buildProgram(log, program, devices);

	// Binary has been copied to program. Can de-allocate buffer now
	delete cb;			
   }

    else{ // Load kernel code and compile it
         finp.close();

         //Load kernel Code
         std::string kernelSource=LoadSource("provider/CL/" + kernelName + ".cl");
         
         // A vector of (data,length) pairs
         cl::Program::Sources sources;
         
         // push kernel
         sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
         
         // collect all kernel sources into a single program
         program = new cl::Program(context, sources);

	 // after building the program, the binary will be valid 
         buildProgram(log, program, devices);	        

         std::vector<char *> binaries(devices.size());
         std::vector<size_t> binarySizes;;
	 
         // get the binary sizes for all OpenCL devices
         program->getInfo<std::vector<size_t>>(CL_PROGRAM_BINARY_SIZES, &binarySizes); 

	 // Now  allocate memory for binaries
	 for(unsigned i = 0; i < binarySizes.size(); i++)
		binaries[i] = new char[binarySizes[i]];

	 // get the binaries for all OpenCL devices
	 program->getInfo<std::vector<char *>>(CL_PROGRAM_BINARIES, &binaries);

	 log->LogInfo( "There are %d binaries and device %d has %d bytes\n", binarySizes.size(), devId, binarySizes[devId]);
	 
	 // write only the binary of interest to output
 	 std::ofstream out;
         fout.open(fname, std::ios::out | std::ios::binary);
	 fout.write(binaries[devId], binarySizes[devId]);
	 fout.close();

	 // de-allocate binary buffers
	 for(unsigned i = 0; i < binarySizes.size(); i++)
		delete binaries[i];

         
    }
    
    // copy data
    setupData->program = program;
    setupData->context = context;
    setupData->device = device;
    

}

void showPlatforms(puzzler::ILog *log, std::vector<cl::Platform> * platforms ){
    
    log->LogInfo("\nFound %d platforms \n", platforms->size());
    
    for(unsigned i=0;i<platforms->size();i++){
        std::string vendor=platforms->at(i).getInfo<CL_PLATFORM_VENDOR>();
        log->LogInfo("\nPlatform %d : %s \n", i, vendor.c_str());

    }
    
}

void showDevices(puzzler::ILog *log, std::vector<cl::Device> * devices){
    
    log->LogInfo("\nFound %d devices \n", devices->size());
    for(unsigned i=0;i<devices->size();i++){
        std::string name=devices->at(i).getInfo<CL_DEVICE_NAME>();
        log->LogInfo("\n  Device %d : %s\n", i, name.c_str());
    }
}

cl::Platform selectPlatform(puzzler::ILog *log, std::vector<cl::Platform> * platforms ){
    int selectedPlatform=0;
    if(getenv("HPCE_SELECT_PLATFORM"))
        selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
    
    log->LogInfo("\nChoosing platform %d \n\n", selectedPlatform);
    return platforms->at(selectedPlatform);
    
}


cl::Device selectDevice(puzzler::ILog *log, std::vector<cl::Device> * devices, uint8_t * selectedDevice){
    
    
    if(getenv("HPCE_SELECT_DEVICE"))
	*selectedDevice = atoi(getenv("HPCE_SELECT_DEVICE"));
    
    log->LogInfo("\nChoosing device %d \n\n", *selectedDevice);
    return devices->at(*selectedDevice);
}

// To go in hpce::your_login, just above StepWorldV3OpenCL
std::string LoadSource(const std::string& fileName)
{
    // Don't forget to change your_login here
    //std::string fullName=fileName;
    
    // Open a read-only binary stream over the file
    std::ifstream src(fileName, std::ios::in | std::ios::binary);
    if(!src.is_open())
        throw std::runtime_error("LoadSource : Couldn't load cl file from '"+fileName+"'.");
    
    // Read all characters of the file into a string
    return std::string(
                       (std::istreambuf_iterator<char>(src)), // Node the extra brackets.
                       std::istreambuf_iterator<char>()
                       );
}



void buildProgram(puzzler::ILog *log, cl::Program * program, const std::vector<cl::Device>& devices){

        try{ // build program
                program->build(devices);
                log->LogInfo( "-------------------------------------\n");
        }catch(...){
                for(unsigned i=0;i<devices.size();i++){

                log->LogInfo("\nLog for device %s \n\n", (devices[i].getInfo<CL_DEVICE_NAME>()).c_str());
                log->LogInfo("\n%s \n\n", program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]).c_str());

                 }
                throw;
        }

}
