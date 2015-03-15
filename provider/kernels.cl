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

__kernel bool kernel_update(int n, const std::vector<bool> &curr, int x, int y) const{
    int neighbours=0;
    for(int dx=-1;dx<=+1;dx++){
        for(int dy=-1;dy<=+1;dy++){
            int ox=(n+x+dx)%n; // handle wrap-around
            int oy=(n+y+dy)%n;
            
            if(curr[oy*n+ox] && !(dx==0 && dy==0))
                neighbours++;
        }
    }
    
    if(curr[n*y+x]){
        // alive
        if(neighbours<2){
            return false;
        }else if(neighbours>3){
            return false;
        }else{
            return true;
        }
    }else{
        // dead
        if(neighbours==3){
            return true;
        }else{
            return false;
        }
    }
}
