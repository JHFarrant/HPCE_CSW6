__kernel void erode_kernel(__global uint* in,__global uint* out,int w, int h)
{
    int x = get_global_id(0);
    
    int S = get_global_size(0);
    
    int column = x%w;
    
    uint cell_above = in[x-w]|( -((x-w) < 0));
    uint cell_below = in[x+w]|( -((x+w) >= S));
    uint cell_left =  in[(x-1)]| -(column==0);
    uint cell_right = in[(x+1)]| -(column==(w-1));
    
    out[x] = min(min(min(cell_above,cell_below),min(cell_left,cell_right)),in[x]);
}

__kernel void dilate_kernel(__global uint* in,__global uint* out, int w, int h)
{
    int x = get_global_id(0);
    
    int  S = get_global_size(0);
    
    int column = x%w;
    
    uint cell_above = in[x-w]&( -((x-w) >= 0));
    uint cell_below = in[x+w] & ( -((x+w) < S));
    uint cell_left =  in[(x-1)]& -(column!=0);
    uint cell_right = in[(x+1)]&-(column!=(w-1));
    
    out[x] = max(max(max(cell_above,cell_below),max(cell_left,cell_right)),in[x]);
}
