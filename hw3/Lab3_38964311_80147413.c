//Pranoto, Timothy: 38964311
//Wongso, Valencia: 80147413

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 400
#define HEADER_SIZE 2

void allocate(char* heap,char* size, int* count);
void freeheap(char* heap, char* num);
void blocklist(char* heap);
void writeheap(char* heap, char* num, char* w, char* mul);
void printheap(char* heap,char* num,char* byte);

int main()
{
  unsigned char* heap=( char*)malloc(sizeof(char)*SIZE);
  int running = 1,count=0; 
  int size =SIZE-HEADER_SIZE;

  //set header 2bytes free space 398
  heap[0]=size>>8;
  heap[1]=size;
  
  while(running == 1){
    char * store[4];
    int set;
    //setting for missing argument error
    for(set=0;set<4;++set)
      store[set]=NULL;

    char input[100];
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
    char *tok = strtok(input," \t\n\r");
    int  i=0;
    while(tok!=NULL&&i<4){
      store[i]=tok;
      tok=strtok(NULL," \t\n\r");
      ++i;
    }
    store[i]=NULL;


    if(strncmp(store[0],"quit",4)==0){
      running = 0;;
    }
    else if(strncmp(store[0],"allocate",8)==0){
      if(store[1]!=NULL){
        if(atoi(store[1])>0&&atoi(store[1])<=size)
          allocate(heap,store[1],&count);
        else
          printf("Error: allocate size must be integer >0 or <= %d\n",size);
      }
      else
        printf("Error: missing argument for allocate\n");
    }
    else if(strncmp(store[0],"free",4)==0){
      if(store[1]!=NULL){
        if(atoi(store[1])<=count&&atoi(store[1])>0)
          freeheap(heap,store[1]);
        else
          printf("Error: invalid block id(1 - %d)\n",count);
      }
      else
        printf("Error: missing argument for free\n");
    }
    else if(strncmp(store[0],"blocklist",9)==0){
      blocklist(heap);
    }
    else if(strncmp(store[0],"writeheap",9)==0){
      if(store[1]!=NULL&&store[2]!=NULL&&store[3]!=NULL){
        if(atoi(store[1])<=count&&atoi(store[1])>0)
          writeheap(heap,store[1],store[2],store[3]);
        else
          printf("Error: invalid block id\n");
      }
      else
        printf("Error: missing argument/s for writeheap\n");
    }
    else if(strncmp(store[0],"printheap",9)==0){
      if(store[1]!=NULL&&store[2]!=NULL){
        if(atoi(store[1])<=count&&atoi(store[1])>0)
          printheap(heap,store[1],store[2]);
        else
          printf("Error: invalid block id\n");
      }
      else
        printf("Error: missing argument/s for printheap\n");
    }
    else
      printf("Invalid command\n");
  }
  free(heap);
  return 0;
}

void allocate(char* heap,char* size, int* count)
{
  int sizeall=atoi(size);
  char* end = heap+SIZE;
  
  while(heap < end){
    
    unsigned char s1=*(heap);
    unsigned char s2=*(heap+1);
    int blockid=s1>>1;
    int a=s1&1;
    int free= a*256+s2;
    if(blockid==0 && free>=sizeall+HEADER_SIZE){
      (*count)++;
      *(heap)=(*(count)<<1)+(sizeall>>8);
      printf("%d\n",(*count));
      *(heap+1)=sizeall;
      
      *(heap+HEADER_SIZE+sizeall)=(free-HEADER_SIZE-sizeall)>>8;
      *(heap+HEADER_SIZE+1+sizeall)=(free-HEADER_SIZE-sizeall);
      
      break;
    }
    else if(blockid==0&&free==sizeall){
      (*count)++;
      *(heap)=(*(count)<<1)+(sizeall>>8);
      printf("%d\n",(*count));
      *(heap+1)=sizeall;
      
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
    unsigned char s1=*(heap);
    unsigned char s2=*(heap+1);
    int blockid=s1>>1;
    int a=s1&1; 
   
    if(blockid==number){
      *(heap)=0+a;
      return;
    }
    int size=a*256+s2;
    heap+=size+HEADER_SIZE;
  }
  printf("Error: block is already free\n");
  
}

void blocklist(char* heap)
{
  char* end = heap+SIZE;
  printf("Header size = %d bytes in each block\n",HEADER_SIZE);
  printf("Size\tAllocated\tStart\t\tEnd\n");
  while(heap<end){
    unsigned char a=*(heap);
    unsigned char b=*(heap+1);
    int blockid =a>>1;
    int h1=a&1;
    int size =  h1*256+b;
    printf("%d",size);
    if(blockid==0)
      printf("\tno");
    else
      printf("\tyes");
    printf("\t\t%p\t%p\n",heap,heap+1+size);
    heap +=size+HEADER_SIZE;
  }
}

void writeheap(char* heap, char* num, char* w, char* mul)
{
  int number = atoi(num);
  int wcount = atoi(mul);
  char* end = heap+SIZE;
  int msg=0;
  int blockid;
  while(heap <end){
    unsigned char a=*(heap);
    unsigned char b=*(heap+1);
    blockid=a>>1;
    int h1=a&1;
    int size = h1*256 +b;
    if(number!=blockid)
      heap+=size+HEADER_SIZE;
    else{
      if(size<wcount){
        wcount=size;
        msg=1;
      }
      break;
    }
  }
  if(blockid==number){
    if(msg==1)
      printf("block dont have enough bytes,writing to %d bytes instead\n",wcount);
    heap+=HEADER_SIZE;
    if(strlen(w)>1)
      printf("second argument have more than 1 char, writing the first one only\n");
    int i=0;
    while(i<wcount){
      *heap=*w;
      ++heap;
      ++i;
    }
  }
  else
    printf("Error: block id is already freed/ unallocated\n"); 
}

void printheap(char* heap,char* num,char* byte)
{
  int number = atoi(num);
  int bytes = atoi(byte);
  char* end=heap+SIZE;
  int blockid;
  int count=1;
  int size;

  while(heap <end){
    unsigned char a=*(heap);
    unsigned char b=*(heap+1);
    blockid=a>>1;
    int h1=a&1;
    size =h1*256 +b;
    
    if(count!=number&&number!=blockid){
      heap+=size+HEADER_SIZE;
      ++count;
    }
    else
      break;
  }
  heap+=HEADER_SIZE;
  int j=0;
  if(bytes>size){
    bytes=size;
    printf("block doesn't have enough bytes, printing %d bytes instead\n",size);
  }
  while(j<bytes){
    printf("%c",*(heap));
    ++heap;
    ++j;
  }
  printf("\n");
  
}
