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
    cl::Device device; // device object
    uint8_t devId = 0; // device id
    std::ifstream fp;
    std::ofstream out;   

 
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
    device=selectDevice(&devices, &devId);
    
    // create context for kernels and memory buffers
    cl::Context context(devices);
    
    
    // try to load from binary (check status, if unsuccessful, load and compile source) 
    cl::Program * program = NULL;
    fp.open("provider/CL/kernels.bin", std::ios::in | std::ios::binary);

    // Check if binary already exists
    if(fp.is_open()){


	fprintf(stderr, "succesfully opened binary file\n");

        // get compiled binary from runtime
        cl::Program::Binaries binaries;
	
   	 // get length of file:
    	fp.seekg (0, fp.end);
    	int length = fp.tellg();
    	fp.seekg (0, fp.beg);


    	// allocate memory
    	char * cb = new char [length];
	fprintf(stderr,"allocating %d bytes\n", length);
	
	fp.read( cb, length);

	fprintf(stderr, "%s", cb);
	binaries.push_back(std::pair<const char *, unsigned>(cb, length));
	fp.close();

	
	fprintf(stderr, "Copied to binaries buffer\n");


	// Create kernel program by also providing the binaries	
        //cl::Program program(context, devices, binaries);
        program = new cl::Program(context, devices, binaries);
	// de-allocate mem

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
	delete cb;			
   }

    else{ // Load kernel code and compile it
         fp.close();
	 //fclose(fp);
         //Load kernel Code
         std::string kernelSource=LoadSource("provider/CL/kernels.cl");
         
         // A vector of (data,length) pairs
         cl::Program::Sources sources;
         
         // push kernel
         sources.push_back(std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
         
         // collect all kernel sources into a single program
         program = new cl::Program(context, sources);

	 program->build(devices);
	        
         std::vector<char *> binaries(1);
         std::vector<size_t> binarySizes;;
	 
	cl_int r; 
         // get the binary
         r = program->getInfo<std::vector<size_t>>(CL_PROGRAM_BINARY_SIZES, &binarySizes); 

	 if (r == CL_SUCCESS)
		fprintf(stderr, "binary size success\n");

	 else
		fprintf(stderr, "binary size failure\n");

	
	 // need to allocate elements for the binaries vector
	  binaries[0] = (char *)malloc(binarySizes[0]);

	 r = program->getInfo<std::vector<char *>>(CL_PROGRAM_BINARIES, &binaries);

	  if (r == CL_SUCCESS)
                fprintf(stderr, "binary success\n");

         else
                fprintf(stderr, "binary failure\n");


       

	 //delete program;
	
	 fprintf(stderr, "There are %d binaries and device %d has %d bytes\n", binarySizes.size(), devId, binarySizes[devId]);
	 

 	 std::ofstream out;
	 //out.open("provider/CL/kernels.bin", std::ofstream::binary);
         out.open("provider/CL/kernels.bin", std::ios::out | std::ios::binary);
	 out.write(binaries[devId], binarySizes[devId]);

	 out.close();


	 // de-allocate binary buffers
	      
    }

    fp.close();
   
    
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


cl::Device selectDevice(std::vector<cl::Device> * devices, uint8_t * selectedDevice){
    
    //int selectedDevice=0;
    
    if(getenv("HPCE_SELECT_DEVICE"))
	*selectedDevice = atoi(getenv("HPCE_SELECT_DEVICE"));
       // selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
    
    fprintf(stderr,"Choosing device %d \n", *selectedDevice);
    return devices->at(*selectedDevice);
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

