//Pranoto, Timothy: 38964311
//Wongso, Valencia: 80147413

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

int main(int argc, char** argv){
  pid_t pid;
  int status; 
  while(1){
    char* store[101];
    char command[101];
    
    pid = waitpid(-1,&status,WNOHANG);

    printf("prompt> ");
    fflush(stdout);

    fgets(command, sizeof(command),stdin);
    if(feof(stdin))
      exit(0);

    if(strlen(command)<2){
      continue;
    }
 
    char *pos;
    if((pos=strchr(command, '\n'))!=NULL)
      *pos ='\0';
    char* tok = strtok(command," \n\t");
    
    int i = 0;
    while(tok!=NULL){
      store[i]=tok;
      tok = strtok(NULL," \n\t");
      ++i;
    }
    store[i]=NULL;
    
    if(strncmp(store[0],"quit",4)==0)
      exit(0);
    else{ 
      int background = 0;
      if(strncmp(store[i-1],"&",1)==0){
        store[i-1]=NULL;
        --i;
        background =1;
      }
      if(strchr(store[i-1],'&')!=NULL){
        char * ptr = store[i-1];
        ptr  = strchr(ptr,'&');
        *ptr ='\0';
        background =1;
      }

      if((pid=fork())==0){
        execvp(store[0],store);
        printf("%s: Command not found.\n",store[0]);
        fflush(stdout);
        exit(0);
      }
      else if(pid<0){
        perror("fork failed\n");
      }
      else if(background==0){
        pid=waitpid(pid,&status,0);
      }
      
    }
  }
  return 0;
}
