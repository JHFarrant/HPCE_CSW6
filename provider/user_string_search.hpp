#ifndef user_string_search_hpp
#define user_string_search_hpp

#include <random>
#include <fstream>

#include "puzzler/core/puzzle.hpp"
#include "puzzler/puzzles/string_search.hpp"




class StringSearchProvider
  : public puzzler::StringSearchPuzzle
{
public:
  virtual void Execute(
           puzzler::ILog *log,
           const puzzler::StringSearchInput *input,
           puzzler::StringSearchOutput *output
           ) const override
  { 
      

      std::vector<std::string> patterns2 = input->patterns;
      //ReferenceExecute(log, input, output);
      std::vector<uint32_t> histogram(input->patterns.size(), 0);
    
      unsigned individual_pattern_length = 1+(unsigned)floor(std::log(input->stringLength)/std::log(10));
      log->LogDebug("individual_pattern_length = %i " ,individual_pattern_length);

      std::string data=MakeString(input->stringLength, input->seed); 
      std::string data2=MakeString(input->stringLength,input->seed); 
      std::cerr<<data<<"\n";

      unsigned size_bools = (unsigned)data.size()*input->patterns.size();
      log->LogDebug("size_bools = %i " ,size_bools);
      std::vector<uint8_t> bool_array(size_bools, false);
      
      struct openCLinstance opencl1;
      setup_opencl(log, &opencl1, input->patterns.size()*sizeof(input->patterns[0]),data.size(),size_bools);
      std::string test_string;

      for (auto const& s : input->patterns) { test_string += s; }
      char * test = (char*) &data.at(0);
      for(int i = 0; i < 10; i++){
      std::cerr<<test[i];
      }
      std::cerr<<"\n";
      std::cerr<<input->patterns[0]<<"\n";
      
      log->LogDebug("Setup opencl : patterns.size()*sizeof(patterns[0])%i , data.size()%i , input->patterns.size()%i " ,input->patterns.size()*sizeof(input->patterns[0]) , data.size() , input->patterns.size());
      test_opencl(log,&opencl1);
      
      opencl1.queue.enqueueWriteBuffer(opencl1.buffer_2, CL_TRUE, 0, input->patterns.size()*sizeof(input->patterns[0]), &test_string.at(0));
      opencl1.queue.enqueueWriteBuffer(opencl1.buffer_3, CL_TRUE, 0, data.size(), &data[0]); 
      opencl1.queue.enqueueWriteBuffer(opencl1.buffer_4, CL_TRUE, 0, size_bools, &bool_array[0]);
      

      // opencl1.queue.enqueueReadBuffer(opencl1.buffer_2, CL_TRUE, 0, input->patterns.size()*sizeof(input->patterns[0]), &patterns2[0]);
      // opencl1.queue.enqueueReadBuffer(opencl1.buffer_3, CL_TRUE, 0, data.size(), &data2[0]); 

      //log->LogDebug("data.compare(data2) = %i " ,data.compare(data2));

       opencl1.kernel_2.setArg(0, opencl1.buffer_2);
       opencl1.kernel_2.setArg(1, opencl1.buffer_3);
       opencl1.kernel_2.setArg(2, opencl1.buffer_4);
       opencl1.kernel_2.setArg(3, (unsigned)data.size());
       opencl1.kernel_2.setArg(4, (unsigned)individual_pattern_length);
       opencl1.kernel_2.setArg(5, opencl1.buffer_5);
       opencl1.kernel_2.setArg(6, opencl1.buffer_6);
       

      // opencl1.kernel_2.setArg(2, opencl1.buffer_4);
      // opencl1.kernel_3.setArg(0, opencl1.buffer_4);
      // opencl1.kernel_3.setArg(1, 3);

      
      cl::NDRange globalSize((unsigned)data.size(),input->patterns.size());
      //cl::NDRange globalSize(1,1);
      opencl1.queue.enqueueNDRangeKernel(opencl1.kernel_2, cl::NDRange(0,0), globalSize, cl::NullRange);
      //opencl1.queue.enqueueNDRangeKernel(opencl1.kernel_3, cl::NDRange(0,0), globalSize, cl::NullRange);
      opencl1.queue.enqueueBarrier();
      /*
      unsigned i=0;
      while(i < input->stringLength){

        for(unsigned p=0; p<input->patterns.size(); p++){

          

          unsigned len=Matches(data, i, input->patterns[p]);
          if(len>0){
  //           log->Log(Log_Debug,[&](std::ostream &dst){
    // dst<<"  Found "<<input->patterns.at(p)<<" at offset "<<i<<", match="<<data.substr(i, len);
  //             });
           // log->LogDebug(" Found %s at offset %i, match= %s",input->patterns.at(p),i,data.substr(i, len).c_str());
      histogram[p]++;
            i += len-1;
            break;
          }
        }

        ++i;
      }

      for(unsigned i=0; i<histogram.size(); i++){
   //      log->Log(Log_Debug, [&](std::ostream &dst){
    //   dst<<input->patterns[i].c_str()<<" : "<<histogram[i];
    // });
        log->LogDebug("%s : %i",input->patterns[i].c_str(),histogram[i]);
      }

    */
      char buf1[4];
      char buf2[4];
      opencl1.queue.enqueueReadBuffer(opencl1.buffer_4, CL_TRUE, 0, size_bools  , &bool_array[0]);
      opencl1.queue.enqueueReadBuffer(opencl1.buffer_5, CL_TRUE, 0, sizeof(char)*4 , &buf1[0]);
      opencl1.queue.enqueueReadBuffer(opencl1.buffer_6, CL_TRUE, 0, sizeof(char)*4 , &buf2[0]);
      std::cerr<<"\nbuf1\n";
      for(int i = 0; i < 4; i++){
      std::cerr<<buf1[i];
      }
      std::cerr<<"\n";
      std::cerr<<"\nbuf2\n";
      for(int i = 0; i < 4; i++){
      std::cerr<<buf2[i];
      }
      std::cerr<<"\n";
      
      //int seen;
      // for(int p = 0; p < input->patterns.size(); p++){
      //   seen = 0;
      //   int i = 0;
      //   while (i < data.size()){
      //   if(bool_array[p*data.size()+i] > 0){
      //     seen++;
      //     i += individual_pattern_length -1;
      //   }
      //   //bin(bool_array[iter]);
      //   //printf("\n");
      //   i++;
      //   }
      //   histogram[p] = seen;
      //   log->LogDebug("%s : %u",input->patterns[p].c_str(),histogram[p]);
      // }
      int i = 0;
      while (i < data.size()){
        for(int p = 0; p < input->patterns.size(); p++)
        {
            if(bool_array[p*data.size()+i] > 0){
              histogram[p]++;   
              std::cerr<<"  Found "<<input->patterns.at(p)<<" at offset "<<i<<", match="<<data.substr(i, bool_array[p*data.size()+i])<<"\n";
              i += bool_array[p*data.size()+i]-1;
              break;
            }
        }
      i++;
      }
         for(int p = 0; p < input->patterns.size(); p++){
        log->LogDebug("%s : %u",input->patterns[p].c_str(),histogram[p]);
      }
      output->occurences=histogram;
  }

private:
  static unsigned Matches(const std::string data, unsigned offset, const std::string pattern) 
  {
      unsigned pos=offset;
      unsigned p=0;

      int seen=0;
      while(p<pattern.size()){
        if(pos>=data.size())
          return 0;

  //fprintf(stderr, " state=%u, off=%u, seen=%u, d=%c, p=%c", p, pos, seen, data[pos], pattern[p]);
  if(pattern.at(p)=='.'){
    pos++;
    p++;
    seen=0;
    //fprintf(stderr, " : match any\n");
  }else if(data.at(pos)==pattern.at(p)){
    seen++;
    pos++;
    //fprintf(stderr, " : match exact %s\n", seen==1?"first":"follow");
  }else{
    if(seen>0){
      p++;
      seen=0;
      // fprintf(stderr, " : move next\n");
    }else{
      //fprintf(stderr, "  Fail\n");
      return 0;
    }
  }
      };
      return pos-offset;
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
static void setup_opencl(puzzler::ILog *log,openCLinstance* opencl1, unsigned patterns_size, unsigned data_size, unsigned size_packed_bools)
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
  
    std::string kernelSource=LoadSource("provider/string_search_kernels.cl");
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
    
    cl::Buffer buffer_2(context, CL_MEM_READ_WRITE, patterns_size);
    cl::Buffer buffer_3(context, CL_MEM_READ_WRITE, data_size);
    cl::Buffer buffer_4(context, CL_MEM_READ_WRITE, size_packed_bools * 4);
    cl::Buffer buffer_5(context, CL_MEM_READ_WRITE, sizeof(char) * 4);
    cl::Buffer buffer_6(context, CL_MEM_READ_WRITE, sizeof(char) * 4);

    opencl1->buffer_1 = buffer_1; //Test Buffer
    opencl1->buffer_2 = buffer_2;
    opencl1->buffer_3 = buffer_3;
    opencl1->buffer_4 = buffer_4;
    opencl1->buffer_5 = buffer_5;
    opencl1->buffer_6 = buffer_6;

    cl::Kernel kernel_1(program, "Add");
    cl::Kernel kernel_2(program, "Matches");
    cl::Kernel kernel_3(program, "Matches2");
    
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
static void  bin(unsigned int v) {
    unsigned int i, s = 1<<((sizeof(v)<<3)-1); // s = only most significant bit at 1
    for (i = s; i; i>>=1) printf("%d", v & i || 0 );
    printf("\n");
}


};

#endif
