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
    
      //setup(log);
      log->LogVerbose("About to start running iterations (total = %d)", input->steps);
      
      unsigned n=input->n;
      std::vector<bool> state=input->state;

      
      std::vector<uint8_t> state8;
      
      boolToUint8t(state, state8);
      
      
      log->Log(puzzler::Log_Debug, [&](std::ostream &dst){
          dst<<"\n";
          for(unsigned y=0; y<n; y++){
              for(unsigned x=0; x<n; x++){
                  dst<<(state.at(y*n+x)?'x':' ');
              }
              dst<<"\n";
          }
	     });
      
      for(unsigned i=0; i<input->steps; i++){
          log->LogVerbose("Starting iteration %d of %d\n", i, input->steps);
          
           // std::vector<bool> next(n*n);
            std::vector<uint8_t> next(n*n);
          
          
          for(unsigned x=0; x<n; x++){
              for(unsigned y=0; y<n; y++){
                  //next[y*n+x]=update(n, state, x, y);
            
                  //next[y*n+x]= kernel_update(n, state, x, y);
                  kernel_update(n, (uint8_t *)&state8[0], x, y, (uint8_t *)&next[0]);
              }
          }
          
         // state=next;
          state8=next;
          
          
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
      
      uint8tToBool(state8, output->state);
      
      log->LogVerbose("Done");
      
  }
    
protected:
    void kernel_update(int n, const uint8_t * curr, int x, int y, uint8_t * next) const{
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
            //dst.push_back( (uint8_t)src.at(it) );
            dst.push_back((bool)src.at(it));
        }
    }
    

};



#endif
