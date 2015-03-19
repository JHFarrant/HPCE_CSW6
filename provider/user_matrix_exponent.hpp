#ifndef user_matrix_exponent_hpp
#define user_matrix_exponent_hpp
//#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include "puzzler/puzzles/matrix_exponent.hpp"

#include <fstream>




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
	struct openCLinstance opencl1;
	setup_opencl(log, &opencl1, input->n);
	log->LogDebug("opencl1->selected_device %d",opencl1.selected_device );
	test_opencl(log,&opencl1);

    std::vector<uint32_t>  hash(input->steps);

      log->LogVerbose("Setting up A and identity");
      auto A=MatrixCreate(input->n, input->seed);
      uint32_t acc = MatrixIdentity(input->n , &opencl1);

      log->LogVerbose("Beginning multiplication");
      hash[0]=acc;
      //OpenCL Here (Include hash vector inside MatrixMul Kernel)
      opencl1.queue.enqueueWriteBuffer(opencl1.buffer_3, CL_TRUE, 0, 4*input->n*input->n, &A[0]);
      for(unsigned i=1; i<input->steps; i++){
        log->LogDebug("Iteration %d", i);
        acc=MatrixMul(input->n, &opencl1);
        hash[i]=acc;
      }
      log->LogVerbose("Done");
	
      output->hashes=hash;

    return;
  }
  
protected:
    static uint32_t MatrixIdentity(unsigned n , openCLinstance* opencl1)
    {
    //Run open_cl Implementation here.
    // run kernel
    // read result
      cl::NDRange globalSize(n, n);
      uint32_t res;
      // for(unsigned i=0; i<n; i++){
      //   res[i*n+i]=1;
      // }
      
      //       for(int row = 0; row < n;row++){
      // 	for(int col = 0; col < n;col++){
      // 		std::cerr<<"["<<res[row*n+col]<<"] ";
      // 	}
      // 	std::cerr<<"\n";
      // }
      // std::cerr<<"\n\n\n";


      //opencl1->queue.enqueueWriteBuffer(opencl1->buffer_2, CL_TRUE, 0, 4 * n * n, &res[0]);
      opencl1->kernel_2.setArg(0, opencl1->buffer_2);
   
      opencl1->queue.enqueueNDRangeKernel(opencl1->kernel_2, cl::NDRange(0,0), globalSize, cl::NullRange);
	  opencl1->queue.enqueueBarrier();
      opencl1->queue.enqueueReadBuffer(opencl1->buffer_2, CL_TRUE, 0, 4, &res);
      //opencl1->queue.enqueueBarrier();

      // for(int row = 0; row < n;row++){
      // 	for(int col = 0; col < n;col++){
      // 		std::cerr<<"["<<res[row*n+col]<<"] ";
      // 	}
      // 	std::cerr<<"\n"; 
      // }
      return res;
    }
    
    static uint32_t MatrixMul(unsigned n,openCLinstance* opencl1)
    {
      //std::vector<uint32_t> res(1, 0);
      uint32_t res;
      //Run open_cl Implementation here.
      
      // for(unsigned r=0; r<n; r++){
      //   for(unsigned c=0; c<n; c++){
      //     for(unsigned i=0; i<n; i++){
      //       res[r*n+c] = Add(res[r*n+c], Mul(a[r*n+i], b[i*n+r]));
      //     }
      //   }
      // }


  //     std::cerr<<"N = "<<n<<"\n";
		// std::cerr<<"-------b-------\n";
  //     for(int row = 0; row < n;row++){
  //     	for(int col = 0; col < n;col++){
  //     		std::cerr<<"["<<b[row*n+col]<<"] ";
  //     	}
  //     	std::cerr<<"\n";
  //     }
  //      std::cerr<<"\n\n\n\n";

  //      	std::cerr<<"-------a-------\n";
  //     for(int row = 0; row < n;row++){
  //     	for(int col = 0; col < n;col++){
  //     		std::cerr<<"["<<a[row*n+col]<<"] ";
  //     	}
  //     	std::cerr<<"\n";
  //     }
  //      std::cerr<<"\n\n\n\n";


      //opencl1->queue.enqueueWriteBuffer(opencl1->buffer_2, CL_TRUE, 0, 4*n*n, &a[0]);
      //opencl1->queue.enqueueWriteBuffer(opencl1->buffer_3, CL_TRUE, 0, 4*n*n, &b[0]);
      //opencl1->queue.enqueueBarrier();

      opencl1->kernel_3.setArg(0, opencl1->buffer_2);
      opencl1->kernel_3.setArg(1, opencl1->buffer_3); 
      opencl1->kernel_3.setArg(2, opencl1->buffer_4);
      opencl1->queue.enqueueNDRangeKernel(opencl1->kernel_3, cl::NDRange(0,0), cl::NDRange(n,n), cl::NullRange);
      opencl1->queue.enqueueBarrier(); 
      opencl1->queue.enqueueReadBuffer(opencl1->buffer_4, CL_TRUE, 0, 4 , &res);
      std::swap(opencl1->buffer_2,opencl1->buffer_4);
	 
	 	// std::cerr<<"-------res-------";
	  // for(int row = 0; row < n;row++){
	  //     	for(int col = 0; col < n;col++){
	  //     		std::cerr<<"["<<res[row*n+col]<<"] ";
	  //     	}
	  //     	std::cerr<<"\n";
	  //     }
	  //     std::cerr<<"\n\n\n\n";

      return res;
    }
    
    static std::string LoadSource(const char *fileName)
{

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
static void setup_opencl(puzzler::ILog *log,openCLinstance* opencl1, unsigned n)
{	
	
	try{
		
		std::vector<cl::Platform> platforms;
			
		cl::Platform::get(&platforms);
		if(platforms.size()==0)
			throw std::runtime_error("No OpenCL platforms found.");
		
		log->LogDebug("Found %i platforms.",platforms.size());	
		//std::cerr<<"Found "<<platforms.size()<<" platforms\n";
		for(unsigned i=0;i<platforms.size();i++){
			std::string vendor=platforms[i].getInfo<CL_PLATFORM_VENDOR>();
			//std::cerr<<"  Platform "<<i<<" : "<<vendor<<"\n";
			log->LogDebug("  Platform : %s",vendor.c_str());	
		}
		
		int selectedPlatform=0;
		if(getenv("HPCE_SELECT_PLATFORM")){
			selectedPlatform=atoi(getenv("HPCE_SELECT_PLATFORM"));
		}
		log->LogDebug("Choosing platform %d",selectedPlatform);
		//std::cerr<<"Choosing platform "<<selectedPlatform<<"\n";
		cl::Platform platform=platforms.at(selectedPlatform);
		
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);	
		if(devices.size()==0){
			throw std::runtime_error("No opencl devices found.\n");
		}
		
		log->LogDebug("Found %i devices.",devices.size());	
		//std::cerr<<"Found "<<devices.size()<<" devices\n";
		for(unsigned i=0;i<devices.size();i++){
			std::string name=devices[i].getInfo<CL_DEVICE_NAME>();
			log->LogDebug("  Device : %s",name.c_str());	
			//std::cerr<<"  Device "<<i<<" : "<<name<<"\n";
		}
		
		int selectedDevice=0;
		if(getenv("HPCE_SELECT_DEVICE")){
			selectedDevice=atoi(getenv("HPCE_SELECT_DEVICE"));
		}
		log->LogDebug("Choosing device %i",selectedDevice);	
		//std::cerr<<"Choosing device "<<selectedDevice<<"\n";
		
		cl::Device device=devices.at(selectedDevice);
		
		opencl1->selected_device = selectedDevice;
		
		log->LogDebug("Setting up context.");
		cl::Context context(devices);

		opencl1->context = context;
		
		log->LogDebug("Compliling Kernels.");
	
		std::string kernelSource=LoadSource("provider/matrix_exponent_kernels.cl");
		kernelSource += "__kernel void Add(__global float *x){ x[get_global_id(0)] += 0.125f; }\n";

		cl::Program::Sources sources(1, std::make_pair(kernelSource.c_str(), kernelSource.size()+1));
		
		cl::Program program(context, sources);
		
		
	    program.build(devices);
	
	    for(unsigned i=0;i<devices.size();i++){
	        std::cerr<<"Log for device "<<devices[i].getInfo<CL_DEVICE_NAME>()<<":\n\n";
	        std::cerr<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i])<<"\n\n";
	 	} 
		
		size_t cbBuffer=4;
		cl::Buffer buffer_1(context, CL_MEM_READ_WRITE, cbBuffer);
		
		cbBuffer=4*n*n;
		cl::Buffer buffer_2(context, CL_MEM_READ_WRITE, cbBuffer);
		cl::Buffer buffer_3(context, CL_MEM_READ_WRITE, cbBuffer);
		cl::Buffer buffer_4(context, CL_MEM_READ_WRITE, cbBuffer);

		opencl1->buffer_1 = buffer_1; //Test Buffer
		opencl1->buffer_2 = buffer_2;
		opencl1->buffer_3 = buffer_3;
		opencl1->buffer_4 = buffer_4;

		cl::Kernel kernel_1(program, "Add");
		cl::Kernel kernel_2(program, "Identity");
		cl::Kernel kernel_3(program, "Multiply");
		//std::cerr<<kernel_1.getInfo<CL_KERNEL_FUNCTION_NAME>()<<"\n\n";
		//std::cerr<<kernel_1.getInfo<CL_KERNEL_NUM_ARGS>()<<"\n\n";
		//std::cerr<<kernel_1.getInfo<CL_KERNEL_REFERENCE_COUNT>()<<"\n\n";
		//std::cerr<<kernel_1.getInfo<CL_KERNEL_CONTEXT>()<<"\n\n";
		//std::cerr<<kernel_1.getInfo<CL_KERNEL_PROGRAM>()<<"\n\n";

		opencl1->kernel_1 = kernel_1;
		opencl1->kernel_2 = kernel_2;
		opencl1->kernel_3 = kernel_3;

		cl::CommandQueue queue(context, device);
		
		opencl1->queue = queue; 
		
	}catch(const std::exception &e){
		std::cerr<<"Exception : "<<e.what()<<std::endl;
		return;
	}
		
	return;
}
static void test_opencl(puzzler::ILog *log,openCLinstance* opencl1){
		log->LogDebug("Testing Opencl.");
	
		unsigned nThreads=1;
		float input=0.5;
		opencl1->kernel_1.setArg(0, opencl1->buffer_1);

		opencl1->queue.enqueueWriteBuffer(opencl1->buffer_1, CL_TRUE, 0, 4, &input);
		opencl1->queue.enqueueBarrier();
			
		opencl1->queue.enqueueNDRangeKernel(opencl1->kernel_1, cl::NDRange(0), cl::NDRange(nThreads), cl::NullRange);
		opencl1->queue.enqueueBarrier();
		
		float output;
		opencl1->queue.enqueueReadBuffer(opencl1->buffer_1, CL_TRUE, 0, 4, &output);
		//std::cerr<<"Output = "<<output<<"\n"<<opencl1->kernel_1.getInfo<CL_KERNEL_FUNCTION_NAME>() <<"\n";
		if(output!=0.625){
			//std::cerr<<"Program executed, but got the wrong output.\n";
			log->LogDebug("Program executed, but got the wrong output. - !!!!!!!!!\n");
			return;
		}else{
			//std::cerr<<"Success, program executed kernel successfully.\n";
			log->LogDebug("Success, program executed test kernel successfully.\n");
		}

}
    
};

#endif
