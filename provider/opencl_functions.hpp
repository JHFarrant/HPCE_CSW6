#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-w" 
    #include "cl.hpp"
#pragma GCC diagnostic pop

struct openCLinstance {
  cl::Kernel kernel_1;
  cl::Kernel kernel_2;
  cl::Kernel kernel_3;
  cl::CommandQueue queue;
  cl::Buffer buffer_1;
  cl::Buffer buffer_2;
  cl::Buffer buffer_3;
  cl::Buffer buffer_4;
  cl::Buffer buffer_5;
  cl::Buffer buffer_6;
  cl::Context context;
  int selected_device;
};