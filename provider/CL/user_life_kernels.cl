__kernel void update_kernel( __global const uchar * curr,
                            __global uchar * next)
{
    
    // get the x,y position within the entire global work group
    uint x=get_global_id(0);
    uint y=get_global_id(1);
    

    // get the size of the iteration space
    uint n=get_global_size(0);

    uint neighbours =0;
    
    uint oy1=(n + y -1)%n; // dy = -1
    uint oy2=(n + y)%n; // dy = 0
    uint oy3=(n + y + 1)%n; // dy = 1
    
    for(int dx=-1;dx<=+1;dx++){
        uint ox=(n+x+dx)%n; // handle wrap-around
        
    
          if(curr[oy1*n+ox])
                neighbours++;
        
        
          if(curr[oy2*n+ox] && dx !=0)
               neighbours++;
        
        
          if(curr[oy3*n+ox])
               neighbours++;
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
