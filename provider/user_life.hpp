#ifndef user_life_hpp
#define user_life_hpp

#include "puzzler/puzzles/life.hpp"
#include "setup.h"


class LifeProvider
  : public puzzler::LifePuzzle
{
public:
  LifeProvider()
  {}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::LifeInput *input,
		       puzzler::LifeOutput *output
		       ) const override {
    
      
      unsigned n=input->n;
      std::vector<uint8_t> state; // life state container
      openCLsetupData setupData;

      
      boolToUint8t(input->state, state); // convert bool vector to uint8t vector
      
      log->LogVerbose("About to start running iterations (total = %d)", input->steps);

      
      // print current state
      log->Log(puzzler::Log_Debug, [&](std::ostream &dst){
          dst<<"\n";
          for(unsigned y=0; y<n; y++){
              for(unsigned x=0; x<n; x++){
                  dst<<(state.at(y*n+x)?'x':' ');
              }
              dst<<"\n";
          }
	     });
      
      // OpenCL application setup
      setup(&setupData);

      // allocate space for buffers on the GPU's local memory
      // Note: 1 byte elements here as the array type is uint8_t
      size_t cbBuffer=n*n; // total bytes to allocate
      cl::Buffer buffBefore(setupData.context, CL_MEM_READ_ONLY, cbBuffer);
      cl::Buffer buffAfter(setupData.context, CL_MEM_WRITE_ONLY, cbBuffer);
      
      // create kernel instance
      cl::Kernel kernel(setupData.program, "update_kernel");
      
      // bind parameters to the kernel to indicate what data
      // they will contain
      kernel.setArg(0, buffBefore);
      kernel.setArg(1, buffAfter);
      
      // Create command queue to
      // synchronise and co-ordinate openCL tasks
      cl::CommandQueue queue(setupData.context, setupData.device);

      
      for(unsigned i=0; i<input->steps; i++){
          

          
          log->LogVerbose("Starting iteration %d of %d\n", i, input->steps);
          
          std::vector<uint8_t> next(n*n); // next state container
          
          for(unsigned x=0; x<n; x++){
              for(unsigned y=0; y<n; y++){
                  kernel_update(n, (uint8_t *)&state[0], x, y, (uint8_t *)&next[0]);
              }
          }
          
          state=next;
          
          
          // The weird form of log is so that there is little overhead
          // if logging is disabled
          log->Log(puzzler::Log_Debug, [&](std::ostream &dst){
              dst<<"\n";
              for(unsigned y=0; y<n; y++){
                  for(unsigned x=0; x<n; x++){
                      dst<<(state[y*n+x]?'x':' ');
                  }
                  dst<<"\n";
              }
          });
      }
      
      log->LogVerbose("Finished steps");
      
      uint8tToBool(state, output->state);
      
      log->LogVerbose("Done");
      
  }
    
protected:
    void kernel_update(unsigned n, const uint8_t * curr, unsigned x, unsigned y, uint8_t * next) const{
        int neighbours=0;
        for(int dx=-1;dx<=+1;dx++){
            for(int dy=-1;dy<=+1;dy++){
                int ox=(n+x+dx)%n; // handle wrap-around
                int oy=(n+y+dy)%n;
                
                if(curr[oy*n+ox] && !(dx==0 && dy==0))
                    neighbours++;
            }
        }
        
        if(curr[n*y+x]){
            // alive
            if(neighbours<2 || neighbours >3)
                next[y*n+x] = false;
 
            else
                next[y*n+x] = true;
        }else{
            // dead
            if(neighbours==3)
                next[y*n+x] = true;
                
            else
                next[y*n+x] = false;
        }
    }

    /*Copies src to dst vector */
    void boolToUint8t(const std::vector<bool> &src,  std::vector<uint8_t> &dst ) const{
        for(unsigned it = 0; it < src.size(); it++){
            dst.push_back( (uint8_t)src.at(it) );
        }
    }
    
    
    /*Copies src to dst vector */
    void uint8tToBool(const std::vector<uint8_t> &src,  std::vector<bool> &dst ) const{
        for(unsigned it = 0; it < src.size(); it++){
            dst.push_back((bool)src.at(it));
        }
    }
    

};



#endif
