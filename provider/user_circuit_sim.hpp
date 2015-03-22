#ifndef user_circuit_sim_hpp
#define user_circuit_sim_hpp

#include "puzzler/puzzles/circuit_sim.hpp"
#include "tbb/task_group.h"
#include "tbb/parallel_for.h"
#include <thread>


class CircuitSimProvider
  : public puzzler::CircuitSimPuzzle
{
public:
  CircuitSimProvider()
  {}

  //~CircuitSimProvider(){}

  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::CircuitSimInput *input,
		       puzzler::CircuitSimOutput *output
		       ) const override {
      // ReferenceExecute(log, input, output);
      
      log->LogVerbose("About to start running clock cycles (total = %d", input->clockCycles);
      std::vector<bool> state=input->inputState;
      const int N = state.size();
      
      
      // update the state every clock
      for(unsigned i=0; i<input->clockCycles; i++){
          log->LogVerbose("Starting iteration %d of %d\n", i, input->clockCycles);
          
          std::vector<bool> res(N);
          
          
          for(unsigned j=0; j<N; j++)
              res[j]=calcSrc(N,input->flipFlopInputs[j], state, input);
          

          
          state=res;
          
          // The weird form of log is so that there is little overhead
          // if logging is disabled
          log->Log(puzzler::Log_Debug,[&](std::ostream &dst) {
              for(unsigned i=0; i<state.size(); i++){
                  dst<<state[i];
              }
          });
      }
      
      log->LogVerbose("Finished clock cycles");
      
      output->outputState=state;

  }
    
private:
    bool calcSrc(unsigned int h, unsigned src, const std::vector<bool> &state, const puzzler::CircuitSimInput *input) const
    {
        if(src < input->flipFlopCount){
            return state.at(src);
        }
        
        else{
            size_t m = h/2;
            
            unsigned nandSrc=src - input->flipFlopCount;
            bool a, b;
            
            if(m < 0.6*input->flipFlopCount){
                a=calcSrc(m, input->nandGateInputs.at(nandSrc).first, state, input);
                b=calcSrc(m, input->nandGateInputs.at(nandSrc).second, state, input);
            } else {
            
                
                tbb::task_group g;
            
                // spawn tasks
                g.run([&]{a=calcSrc(m, input->nandGateInputs.at(nandSrc).first, state, input);});
                g.run([&]{b=calcSrc(m, input->nandGateInputs.at(nandSrc).second, state, input);});

                g.wait();
            }
            
            // wait for both tasks to complete
            return !(a&&b);
        }
    }
    
   
};




#endif
