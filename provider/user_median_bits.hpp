#ifndef user_median_bits_hpp
#define user_median_bits_hpp

#include "puzzler/puzzles/median_bits.hpp"
#include "tbb/parallel_sort.h"


class MedianBitsProvider
  : public puzzler::MedianBitsPuzzle
{
public:
  MedianBitsProvider()
  {}

  virtual void Execute(
           puzzler::ILog *log,
           const puzzler::MedianBitsInput *input,
           puzzler::MedianBitsOutput *output
           ) const override {
    //ReferenceExecute(log, input, output);
    log->LogInfo("Generating bits.");
      // double tic=now();
      
      std::vector<uint32_t> temp(input->n);
      uint32_t * temp_ptr = &temp[0];

      tbb::parallel_for((unsigned)0, (unsigned)input->n, [=](unsigned i){
      //for(unsigned i=0; i<input->n; i++){
        uint32_t x=i*(7 + input->seed);
        uint32_t y=0;
        uint32_t z=0;
        uint32_t w=0;
        
        for(unsigned j=0; j<(unsigned)(std::log(16+input->n)/std::log(1.1)); j++){
          uint32_t t = x ^ (x << 11);
          x = y; y = z; z = w;
          w = w ^ (w >> 19) ^ t ^ (t >> 8);
        }
        
        temp_ptr[i] = w;
      });
      
      // log->LogInfo("Finding median, delta=%lg", now()-tic);
      // tic=now();
      log->LogInfo("Running parallel_sort");
      tbb::parallel_sort(temp.begin(), temp.end());
      //std::sort(temp.begin(), temp.end());
      
      output->median=temp[temp.size()/2];  
      
      // log->LogInfo("Done, median=%u (%lg), delta=%lg", output->median, output->median/pow(2.0,32), now()-tic);
  } 

};

#endif
