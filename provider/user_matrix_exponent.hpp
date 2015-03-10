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
    
};

#endif
