#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 400
#define HEADER_SIZE 3

int* binary(int a);
void allocate(char* heap,char* size, int* count);
void freeheap(char* heap, char* num);
void blocklist(char* heap);
void writeheap(char* heap, char* num, char* w, char* mul);
void printheap(char* heap,char* num,char* byte);

int main()
{
  unsigned char* heap=( char*)malloc(sizeof(char)*SIZE);
  int running = 1,count=1; 
  int size =SIZE-HEADER_SIZE;

  //set header 2bytes free space 398
  heap[0]=0;
  heap[1]=size>>8;
  heap[2]=size;
  
  while(running == 1){
    char * store[5];
    int set;
    //setting for missing argument error
    for(set=0;set<5;++set)
      store[set]=NULL;
    char input[50];
    //prompt and input
    printf(">");
    fgets(input,sizeof(input),stdin);
    if(feof(stdin))
      exit(0);    
    if(strlen(input)<2)//check for empty input
      continue;
    char* pos;
    if((pos=strchr(input,'\n'))!=NULL)
      *pos='\0';
    char *tok = strtok(input," ");
    int  i=0;
    while(tok!=NULL){
      store[i]=tok;
      tok=strtok(NULL," ");
      ++i;
    }
    store[i]=NULL;


    if(strncmp(store[0],"quit",4)==0){
      running = 0;;
    }
    else if(strncmp(store[0],"allocate",8)==0){
      if(atoi(store[1])>0)
        allocate(heap,store[1],&count);
      else
        printf("Error: allocate size must be >0\n");
    }
    else if(strncmp(store[0],"free",4)==0){
      if(atoi(store[1])<=count&&atoi(store[1])>0)
        freeheap(heap,store[1]);
      else
        printf("Error: invalid block number\n");
    }
    else if(strncmp(store[0],"blocklist",9)==0){
      blocklist(heap);
    }
    else if(strncmp(store[0],"writeheap",9)==0){
      if(atoi(store[1])<=count&&atoi(store[1])>0)
        writeheap(heap,store[1],store[2],store[3]);
      else
        printf("Error: invalid block number\n");
    }
    else if(strncmp(store[0],"printheap",9)==0){
      if(atoi(store[1])<=count&&atoi(store[1])>0)
        printheap(heap,store[1],store[2]);
      else
        printf("Error: invalid block number\n");
    }
  }
  free(heap);
  return 0;
}

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


void allocate(char* heap,char* size, int* count)
{
  int sizeall=atoi(size);
  char* end = heap+SIZE;
  
  while(heap < end){
    
    unsigned char s1=*(heap+1);
    unsigned char s2=*(heap+2);
    int free= (s1<<8)+s2;
    if(*(heap)==0 && free>=sizeall){
      
      *(heap)=*count;
      printf("%d\n",(*count)++);
      *(heap+1)=sizeall>>8;
      *(heap+2)=sizeall;
      
      if(free>sizeall+HEADER_SIZE){
        *(heap+HEADER_SIZE+sizeall)=0;
        *(heap+HEADER_SIZE+1+sizeall)=(free-HEADER_SIZE-sizeall)>>8;
        *(heap+HEADER_SIZE+2+sizeall)=(free-HEADER_SIZE-sizeall);
      }
      break;
    }
    heap+=free+HEADER_SIZE;
  }
}

void freeheap(char* heap, char* num)
{
  int number =atoi(num);
  char* end= heap+SIZE;
  while(heap<end){
    if(*(heap)==number){
      *(heap)=0;
      return;
    }
    unsigned char s2=*(heap+2);
    int size=(*(heap+1)<<8)+s2;
    heap+=size+HEADER_SIZE;
  }
  printf("Error: block is already free\n");
  
}

void blocklist(char* heap)
{
  char* end = heap+SIZE;
 
  printf("Size\tAlloc\tStart\t\tEnd\n");
  while(heap<end){
    unsigned char a=*(heap+1);
    unsigned char b=*(heap+2);
    int size =  (a<<8)+b;
    printf("%d",size);
    if(*(heap) ==0)
      printf("\tno");
    else
      printf("\tyes");
    printf("\t%p\t%p\n",heap+HEADER_SIZE,heap+2+size);
    heap +=size+HEADER_SIZE;
  }
}

void writeheap(char* heap, char* num, char* w, char* mul)
{
  int number = atoi(num);
  int wcount = atoi(mul);
  char* end = heap+SIZE;
  while(heap <end&&number!=(*heap)){
    unsigned char b=*(heap+2);
    int size = (*(heap+1)<<8) +b;
    heap+=size+HEADER_SIZE;
  }
  if(*heap==number){
    heap+=HEADER_SIZE;
    int i=0;
    while(i<wcount){
      *heap=*w;
      ++heap;
      ++i;
    }
  }
  else
    printf("Error: block number is not available\n"); 
}

void printheap(char* heap,char* num,char* byte)
{
  int number = atoi(num);
  int bytes = atoi(byte);
  char* end=heap+SIZE;
  
  while(heap <end&&number!=(*heap)){
    unsigned char b=*(heap+2);
    int size = (*(heap+1)<<8) +b;
    heap+=size+HEADER_SIZE;
  }

  heap+=HEADER_SIZE;
  int j=0;
  while(j<bytes){
    printf("%c",*(heap));
    ++heap;
    ++j;
  }
  printf("\n");
}
