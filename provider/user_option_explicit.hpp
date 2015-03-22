#ifndef user_option_explicit_hpp
#define user_option_explicit_hpp

#include <random>

#include "puzzler/core/puzzle.hpp"
#include "puzzler/puzzles/option_explicit.hpp"
#include "tbb/task_group.h"
#include "tbb/parallel_for.h"

class OptionExplicitProvider
  : public puzzler::OptionExplicitPuzzle
{
public:
  virtual void Execute(
		       puzzler::ILog *log,
		       const puzzler::OptionExplicitInput *input,
		       puzzler::OptionExplicitOutput *output
		       ) const override {
    //ReferenceExecute(log, input, output);
      int n=input->n;
      double u=input->u, d=input->d;
      std::vector<double> VUs(n);
      std::vector<double> VDs(n);
      std::vector<double> Ws(n*2+1);
      
      log->LogInfo("Params: u=%lg, d=%lg, wU=%lg, wM=%lg, wD=%lg", input->u, input->d, input->wU, input->wM, input->wD);
      
      std::vector<double> state(n*2+1);
      double vU=input->S0, vD=input->S0;
      state[input->n]=std::max(vU-input->K,0.0);
      
      double wU=input->wU, wD=input->wD, wM=input->wM;
      
      VUs[0] = vU;
      VDs[0] = vD;
      
      tbb::parallel_for<unsigned>(1, n,[&](unsigned i){
          vU=vU*u;
          vD=vD*d;
          
          // save vU and vD constants for later on
          VUs[i] = vU;
          VDs[i] = vD;
          state[n+i]=std::max(vU-input->K,0.0);
          state[n-i]=std::max(vD-input->K,0.0);
          
      });
     // Ws[n] = wU*state[n+1]+wM*state[n]+wD*state[n-1];
      
    
      for(int t=n-1; t>=0; t--){
          std::vector<double> tmp=state;
          
          //vU=input->S0, vD=input->S0;
          
          tbb::parallel_for<unsigned>(0, n, [&](unsigned i){
          //for(int i=0; i<n; i++){
              double vCU=wU*state[n+i+1]+wM*state[n+i]+wD*state[n+i-1];
              double vCD=wU*state[n-i+1]+wM*state[n-i]+wD*state[n-i-1];
              //double vCU = Ws[n+i];
             // double vCD = Ws[n-i];
              vCU=std::max(vCU, VUs[i]-input->K);
              vCD=std::max(vCD, VDs[i]-input->K);
              tmp[n+i]=vCU;
              tmp[n-i]=vCD;
              
             // vU=vU*u;
              //vD=vD*d;
          });
          
          state=tmp;
      }
      
      /*
      tbb::parallel_for<unsigned>(0, n, [&](unsigned t){
          
          std::vector<double> tmp=state;
          vU=input->S0, vD=input->S0;
          
          for(int i=0; i<n; i++){
              double vCU=wU*state[n+i+1]+wM*state[n+i]+wD*state[n+i-1];
              double vCD=wU*state[n-i+1]+wM*state[n-i]+wD*state[n-i-1];
              vCU=std::max(vCU, vU-input->K);
              vCD=std::max(vCD, vD-input->K);
              tmp[n+i]=vCU;
              tmp[n-i]=vCD;
              
              vU=vU*u;
              vD=vD*d;
          }
          state=tmp;
      });*/
      
      output->steps=n;
      output->value=state[n];
      
      log->LogVerbose("Priced n=%d, S0=%lg, K=%lg, r=%lg, sigma=%lg, BU=%lg : value=%lg", n, input->S0, input->K, input->r, input->sigma, input->BU, output->value);
  

  }

};

#endif
