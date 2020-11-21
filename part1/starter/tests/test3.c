#include <stdio.h>
#include <malloc.h>


int main(){

  for(int i= 0; i < 100; i++){
      int* data =  (int*)malloc(8);    
      int* data1 = (int*)malloc(16);
      int* data2 = (int*)malloc(32);
      int* data3 = (int*)malloc(64);
      int* data4 = (int*)malloc(128);
      
      free(data);
      free(data1);
      free(data2);
      free(data3);
      free(data4);
  }

  return 0;
}
