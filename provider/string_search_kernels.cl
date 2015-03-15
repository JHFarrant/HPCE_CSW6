__kernel void Matches(__global char* patterns,  __global char* data,  __global uchar* bools, uint data_size,  uint individual_pattern_length, __global char* buf1,__global char* buf2)
{

    uint pos= get_global_id(0);
    uint pat_num = get_global_id(1);
    
    uint n=get_global_size(0);
    uint n2=get_global_size(1);
    uint index = (pat_num*n+pos);

    
    //if(index%2 == 1)bools[index] = true;
    ; /*
    ; bools[0] = (unsigned) 123UL;
    ; bools[1] = (unsigned)(index/32);
    
    ; bools[2] = (unsigned)(index%32);
    ; bools[3] = pat_num;
    ; bools[4] = data_pos;
    ; bools[5] = n;
    ; bools[6] = index;
    ; bools[7] = n2;
    

    ; buf1[0] = data[0];
    ; buf1[1] = data[1];
    ; buf1[2] = data[2];
    ; buf1[3] = data[3];
    ; buf2[0] = data[4];
    ; buf2[1] = data[5];  
    ; buf2[2] = 'x';
    ; buf2[3] = 'x';    
    ; */

    bools[index] = 0;
    unsigned p=0;

    int seen=0;
    ; /*
    ; while(p<individual_pattern_length){
    ;   if(patterns[pat_num*individual_pattern_length+p]!='.' && patterns[pat_num*individual_pattern_length+p]!=data[pos]){
    ;     return;
    ;   }
    ;   p++;
    ;   pos++;
    ; }
    ; bools[index] = true;
    ; */


   while(p<individual_pattern_length){
      if(pos>=n)
        return;

  //fprintf(stderr, " state=%u, off=%u, seen=%u, d=%c, p=%c", p, pos, seen, data[pos], pattern[p]);
  if(patterns[pat_num*individual_pattern_length+p]=='.'){
    pos++;
    p++;
    seen=0;
    //fprintf(stderr, " : match any\n");
  }else if(data[pos]==patterns[pat_num*individual_pattern_length+p]){
    seen++;
    pos++;
    //fprintf(stderr, " : match exact %s\n", seen==1?"first":"follow");
  }else{
    if(seen>0){
      p++;
      seen=0;
      // fprintf(stderr, " : move next\n");
    }else{
      //fprintf(stderr, "  Fail\n");
      return;
    }
  }
      };
      bools[index] = pos-get_global_id(0);
  




}
