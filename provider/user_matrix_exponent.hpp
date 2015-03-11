#ifndef user_matrix_exponent_hpp
#define user_matrix_exponent_hpp

#include "puzzler/puzzles/matrix_exponent.hpp"
struct openCLinstance {
  cl::Kernel kernel_1;
  cl::Kernel kernel_2;
  cl::CommandQueue queue;
  cl::Buffer buffer_1;
  cl::Buffer buffer_2;
  cl::Buffer buffer_3;
  cl::Context context;
  int selected_device;
} ;






class MatrixExponentProvider
  : public puzzler::MatrixExponentPuzzle
{
public:
  MatrixExponentProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::MatrixExponentInput *input,
		       puzzler::MatrixExponentOutput *output
		       ) const override {

    std::vector<uint32_t> hash(input->steps);

      log->LogVerbose("Setting up A and identity");
      auto A=MatrixCreate(input->n, input->seed);
      auto acc=MatrixIdentity(input->n);

      log->LogVerbose("Beginning multiplication");
      hash[0]=acc[0];
      //OpenCL Here (Include hash vector inside MatrixMul Kernel)
      for(unsigned i=1; i<input->steps; i++){
        log->LogDebug("Iteration %d", i);
        acc=MatrixMul(input->n,acc, A);
        hash[i]=acc[0];
      }
      log->LogVerbose("Done");
	
      output->hashes=hash;

    return;
  }
  
protected:
    static std::vector<uint32_t> MatrixIdentity(unsigned n)
    {
    //Run open_cl Implementation here.
      std::vector<uint32_t> res(n*n,0);
      for(unsigned i=0; i<n; i++){
        res[i*n+i]=1;
      }
      return res;
    }
    
    static std::vector<uint32_t> MatrixMul(unsigned n, std::vector<uint32_t> a, std::vector<uint32_t> b)
    {
      std::vector<uint32_t> res(n*n, 0);
      //Run open_cl Implementation here.
      for(unsigned r=0; r<n; r++){
        for(unsigned c=0; c<n; c++){
          for(unsigned i=0; i<n; i++){
            res[r*n+c] = Add(res[r*n+c], Mul(a[r*n+i], b[i*n+r]));
          }
        }
      }
      return res;
    }
    
    static std::string LoadSource(const char *fileName)
{
    // Don't forget to change your_login here
    std::string baseDir="src/jf1711";
    if(getenv("HPCE_CL_SRC_DIR")){
        baseDir=getenv("HPCE_CL_SRC_DIR");
    }

    std::string fullName=baseDir+"/"+fileName;

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
static openCLinstance* setup_opencl()
{
	try{
		openCLinstance opencl1;
		
		std::vector<cl::Platform> platforms;
			
		cl::Platform::get(&platforms);
		if(platforms.size()==0)
			throw std::runtime_error("No OpenCL platforms found.");
		
		std::cerr<<"Found "<<platforms.size()<<" platforms\n";
		for(unsigned i=0;i<platforms.size();i++){
			std::string vendor=platforms[i].getInfo<CL_PLATFORM_VENDOR>();
			std::cerr<<"  Platform "<<i<<" : "<<vendor<<"\n";
		}
		
		int selectedPlatform=0;
		if(getenv("HPCE_SELECT_PLATFORM")){
			selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
		}
		std::cerr<<"Choosing platform "<<selectedPlatform<<"\n";
		cl::Platform platform=platforms.at(selectedPlatform);
		
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);	
		if(devices.size()==0){
			throw std::runtime_error("No opencl devices found.\n");
		}
			
		std::cerr<<"Found "<<devices.size()<<" devices\n";
		for(unsigned i=0;i<devices.size();i++){
			std::string name=devices[i].getInfo<CL_DEVICE_NAME>();
			std::cerr<<"  Device "<<i<<" : "<<name<<"\n";
		}
		
		int selectedDevice=0;
		if(getenv("HPCE_SELECT_DEVICE")){
			selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
		}
		std::cerr<<"Choosing device "<<selectedDevice<<"\n";
		cl::Device device=devices.at(selectedDevice);
		
		opencl1.selected_device = selectedDevice;
		
		cl::Context context(devices);
		
		opencl1.context = context;
		
		std::string kernelSource="";
		
		cl::Program::Sources sources(1, std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
		
		cl::Program program(context, sources);
		program.build(devices);
		
		unsigned nThreads=1;
		
		size_t cbBuffer=4;
		cl::Buffer buffer(context, CL_MEM_READ_WRITE, cbBuffer);
		
		cl::Kernel kernel_1(program, "Add");
		
		opencl1.kernel_1 = kernel_1;
		
		cl::CommandQueue queue(context, device);
		
		opencl1.queue = queue; 
		
	}catch(const std::exception &e){
		std::cerr<<"Exception : "<<e.what()<<std::endl;
		return 1;
	}
		
	return opencl1;
}

    
};

#endif
