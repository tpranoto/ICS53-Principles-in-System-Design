#include <stdio.h>

struct record{
  char *name[100];
  char* address[100];
};

void command(char* file, struct record *data){
  FILE* ifp;
  ifp= fopen(file,"r");
  
  int j=0;
  while(fscanf(ifp,"%s\t%s",data[j].name,data[j].address)!=EOF){
    printf("%s and %s\n", data[j].name,data[j].address);
    ++j;
  }
  fclose(ifp);
}

int main(){
  struct record data[20];
  command("test1.rec",data);
  return 0;
}
