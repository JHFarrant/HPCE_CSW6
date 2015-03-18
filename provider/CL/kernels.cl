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

__kernel void update_kernel( __global const uchar * curr,  __global uchar * next)
{
    
    // get the x,y position within the entire global work group
    uint x=get_global_id(0);
    uint y=get_global_id(1);
    
    // get the size of the iteration space
    uint n=get_global_size(0);

    // we know that n > 0 and  0<=x<n.
    // Also this kernel will only run if scale > 0.
    // As n = scale, the smallest value
    // of ox when this kernel runs is when n = 1 ,
    // x =0 and dx = -1  <=> ox = 0
    // same for oy. So ox and oy are always +ve. Therefore
    // only need to check for wrap around on the right side.
    
    
    uint neighbours=0;
    for(int dx=-1;dx<=+1;dx++){
        for(int dy=-1;dy<=+1;dy++){
            uint ox=(n+x+dx)%n; // handle wrap-around
            uint oy=(n+y+dy)%n;
           /* uint ox = n+x+dx;
            uint oy = n+y+dy;
            
            // perform wrap around
            // if we reach the maximum co-ordinate we move back
            // to 0
            ox =  (ox == 2*n) ? 0:((ox > n-1) ? ox - n: ox);
            oy =  (oy == 2*n) ? 0:((oy > n-1) ? oy - n: oy);*/
            
            
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
