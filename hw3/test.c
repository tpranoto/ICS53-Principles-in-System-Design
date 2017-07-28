#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* binary(int a)
{
  static int temp[8];
  int j;
  for(j=0;j<8;++j)
    temp[j]=0;
  int i=7,rem;
  while(a!=0){
    temp[i]=a%2;   
   
    a=a/2;
    --i;
  }
  
  return temp;
}

int headerid(int* bin, int start,int end)
{
  int p2=start,i=0,ret=0;
  while(i<=end){
    ret+=bin[i]*p2;
    p2*=2;
    ++i;
  }
  return ret;
}


int main()
{
  const char* testing="a";
  const char* t2="Aaasd"; 
  int alloc=3;
  //int and=testing&1;
  //unsigned char rest = testing<<7;
  
  printf("%d\n",atoi(t2));
  
  

}
