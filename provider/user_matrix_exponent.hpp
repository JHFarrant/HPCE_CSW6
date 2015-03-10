#ifndef user_matrix_exponent_hpp
#define user_matrix_exponent_hpp

#include "puzzler/puzzles/matrix_exponent.hpp"

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
      for(unsigned i=1; i<input->steps; i++){
        log->LogDebug("Iteration %d", i);
        acc=MatrixMul(input->n,acc, A);
        hash[i]=acc[0];
      }
      log->LogVerbose("Done");

      output->hashes=hash;

    return;
  }

};

#endif
