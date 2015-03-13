#pragma OPENCL EXTENSION cl_khr_fp64: enable

uint mul_num(uint a, uint b)
    {
      long tmp=a*b;
      return (uint)(tmp%2147483647UL);
    }

uint add_num(uint a, uint b)
    {
      long tmp=a+b;
      return (uint)(tmp%2147483647UL);
    }

__kernel void Identity(__global uint* in)
{
    uint x=get_global_id(0);
    uint y=get_global_id(1);
    uint w=get_global_size(0);
    
    if( x == y){
        in[y*w + x] = 1;
    }
    else
    {
        in[y*w + x] = 0;
    }
}

__kernel void Multiply(__global uint* a,__global uint* b, __global uint* res)
{
    uint c=get_global_id(0);
    uint r=get_global_id(1);
    uint n=get_global_size(0);
    res[r*n+c] = 0;
    for(unsigned i=0; i<n; i++){
        res[r*n+c] = add_num(res[r*n+c], mul_num(a[r*n+i], b[i*n+r]));
        }

}

    