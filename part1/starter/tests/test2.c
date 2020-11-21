// A second test
#include <stdio.h>
#include <malloc.h>


int main(){

  for(int i= 0; i < 100; i++){
      int* data = (int*)malloc(1024);
      free(data);
  }

  return 0;
}
