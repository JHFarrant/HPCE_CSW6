#ifndef user_life_hpp
#define user_life_hpp

#include "puzzler/puzzles/life.hpp"
//#include <cmath>
#include "setup.h"

class LifeProvider: public puzzler::LifePuzzle{
public:
  LifeProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::LifeInput *input,
		       puzzler::LifeOutput *output
		       ) const override {

    
      
      unsigned n=input->n;
      //std::vector<uint8_t> state; // life state container
      openCLsetupData setupData;

      
      size_t cbytes = std::ceil((double)n*n/8.0); // determine total bytes needed to store flags
      std::vector<uint8_t> state(cbytes, 0); // allocate a sufficiently large vector.
      
      //std::vector<bool> s = input->state;
      packBits(input->state, state);
      
      
      //http://stackoverflow.com/questions/670308/alternative-to-vectorbool
      //http://stackoverflow.com/questions/4441280/does-opencl-support-boolean-variables
      //boolToUint8t(input->state, state); // convert bool vector to uint8t vector
      
      log->LogVerbose("About to start running iterations (total = %d)", input->steps);

      
      // print current state
      log->Log(puzzler::Log_Debug, [&](std::ostream &dst){
          
          for(unsigned x=0; x<n*n; x++){
                 if (x%n == 0)
                        dst<<"\n";
              
                  bool r = getBit(x, state);
                  dst<<(r ?'x':' ');

                  
          }
      });
      
      // OpenCL application setup
      setup(&setupData);

      // allocate space for buffers on the GPU's local memory
      // Note: 1 byte elements here as the array type is uint8_t
      //size_t cbBuffer=n*n; // total bytes to allocate
      size_t cbBuffer = cbytes;
      cl::Buffer buffBefore(setupData.context, CL_MEM_READ_WRITE, cbBuffer);
      cl::Buffer buffAfter(setupData.context, CL_MEM_READ_WRITE, cbBuffer);
      
      // create kernel instance
      cl::Kernel kernel(setupData.program, "update_kernel");
      
      
      // Create command queue to
      // synchronise and co-ordinate openCL tasks
      cl::CommandQueue queue(setupData.context, setupData.device);

      //std::vector<uint8_t> next(n*n); // next state container
      
      // copy the current state over to the GPU
      //cl::Event evCopiedState;
      queue.enqueueWriteBuffer(buffBefore, CL_TRUE, 0, cbBuffer, &state[0]);
      
      // Set up iteration space
      cl::NDRange offset(0, 0);               // Always start iterations at x=0, y=0
      cl::NDRange globalSize(n, n);   // Global size must match the original loops
     // cl::NDRange localSize=cl::NullRange;    // We don't care about local size
      cl::NDRange localSize(1,1);
      
      for(unsigned i=0; i<input->steps; i++){
          
          
          log->LogVerbose("Starting iteration %d of %d\n", i, input->steps);
          
          
          // bind parameters to the kernel to indicate what data
          // they will contain
          kernel.setArg(0, buffBefore);
          kernel.setArg(1, buffAfter);
          
          // execute kernel
          queue.enqueueNDRangeKernel(kernel, offset, globalSize, localSize);

          // create a synchronisation point within the command queue
          queue.enqueueBarrier(); // <- new barrier here
          
          // swap from output to input
          std::swap(buffBefore, buffAfter);
          
          
      } // end for
      
      
      
      // write back results
      queue.enqueueReadBuffer(buffBefore, CL_TRUE, 0, cbBuffer, &state[0]);
      
      
      // The weird form of log is so that there is little overhead
      // if logging is disabled
      log->Log(puzzler::Log_Debug, [&](std::ostream &dst){
          
          dst << "Result:\n";
          
          for(unsigned x=0; x<n*n; x++){
              if (x%n == 0)
                  dst<<"\n";
              dst<<( getBit(x, state)?'x':' ');
          }
          
      });

      
      
      log->LogVerbose("Finished steps");
     
      // uint8tToBool(state, output->state);
      // write bit flags back to vector
      unpackBits(n, state, output->state);
      
      log->LogVerbose("Done");
      log->LogVerbose("========================================");

  }
    
  
    
protected:
    
    /*Copies src to dst vector */
    void boolToUint8t(const std::vector<bool> &src,  std::vector<uint8_t> &dst ) const{
        for(unsigned it = 0; it < src.size(); it++){
            dst.push_back( (uint8_t)src.at(it) );
        }
    }
    
    
    /*Copies src to dst vector */
    void uint8tToBool(const std::vector<uint8_t> &src,  std::vector<bool> &dst ) const{
        /*for(unsigned it = 0; it < src.size(); it++){
            dst.push_back((bool)src.at(it));
        }*/
       for(std::vector<uint8_t>::const_iterator it = src.begin(); it !=src.end(); ++it){
            dst.push_back((bool)*it);
        }
    }
    
    /* Rounds a number to the nearest multiple of 8 to determine 
     the actual number of bits needed to contain the state variable*/
   /* rndNearestByte(unsigned n){
        
        unsigned r = n % 8;
        
        r == 0 ?  n: (n + 8 - r);
            
    }*/
    
    void packBits(const std::vector<bool> &src,  std::vector<uint8_t> &dst) const{
        
        for(unsigned i = 0; i < src.size(); i++){
            
            bool f = src.at(i);
            
            // we know that the flag will always be in the LSB (bit 0) of f
            // so we only want to copy that bit without changing the others
            // (f | 0xFE) << p;
            
            // dst[round((p+1)/8)] = 0xFF ^ (-(f << p%8) ^ 0xFF) & (1 << n) ;
            
            // change the i-th bit of 0xFF to the current flag
            unsigned n = round(i/8);
            dst[n] ^= (-f ^ dst[n]) & (1 << (i%8)) ;
            // p++;
            
        
        }
        
    }
    
    bool getBit(unsigned i, const std::vector<uint8_t> &state ) const{
        
        // find word position
        unsigned w = round(i/8);
        
        // find bit position
        unsigned b = i%8;
        
        bool r = (state[w] >> b ) & 0x1;

        return r;

    }
    
    void unpackBits(unsigned n, const std::vector<uint8_t> &src,  std::vector<bool> &dst) const{
        
        // only need to read in n bits
        for(unsigned i=0; i < n*n; i++){

            dst.push_back( getBit(i, src));
            
        }
    }
    

};



#endif
