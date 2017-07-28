//Timothy Pranoto 38964311
//ICS 53 Lab1


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct record{
  char name[80];
  char address[80];
  char number[80];
};

int readCommand(char *fileName,struct record *data)
{
  FILE* ifp;
  ifp= fopen(fileName,"r");
  if(ifp==NULL)
    fprintf(stderr,"Can't open %s\n",fileName);

  int i=0;
  while(fscanf(ifp,"%[^\t\n]\t%[^\t\n]\t%[^\t\n]\n", data[i].name, data[i].address, data[i].number)!=EOF)
    ++i;
  
  fclose(ifp);
  return i;
}

void writeCommand(char *fileName, struct record *data, int size)
{
  FILE* ofp;
  ofp=fopen(fileName,"w");
  if(ofp==NULL)
    fprintf(stderr,"Can't open %s\n",fileName);
  
  int i=0;
  while(i<size){
    fprintf(ofp,"%s\t%s\t%s\n", data[i].name, data[i].address, data[i].number);
    ++i;
  }
  fclose(ofp);
}

void printCommand(char* fileName, struct record *data, int size)
{
  int a=0;
  while(a<size){
    printf("%d\t%s\t%s\t%s\n", a+1, data[a].name, data[a].address, data[a].number);
    ++a;
  }
}

void deleteCommand(char* number, struct record* data, int size)
{
  int index =atoi(number)-1;
  
  while(index<size-1){
    memcpy(&data[index],&data[index+1],sizeof*data);
    ++index;
  }
  memset(&data[size-1],0,sizeof*data);
}

int main()
{
  struct record data[101];
  int size=0;

  while(1)
  {
    char * store[10];
    char input[50];

    printf(">");
    fgets(input,sizeof(input),stdin);
    if(feof(stdin))
      exit(0);

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

      exit(0);

    }
    else if(strncmp(store[0],"read",4)==0){

      size= readCommand(store[1],data);

    }
    else if(strncmp(store[0],"write",5)==0){
    
      writeCommand(store[1], data, size);
      
    }
    else if(strncmp(store[0],"print",5)==0){
      
      printCommand(store[1], data, size);

    }
    else if(strncmp(store[0],"delete",6)==0){
      deleteCommand(store[1], data, size);
      --size;
    }
    else
     printf("Invalid input\n");
  }

  return 0;  
}
