#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define NUM_SONG 1024
typedef struct
{
    char title[40];
    char* artist;
    int year; 
}Song;

FILE * open_file(char* fileName, char* mode)
{
    FILE* fp;
    fp= fopen (fileName, mode);
    if(fp==NULL){
        fprintf(stderr, "Can't open %s\n", fileName);
        exit(1);
    }
    return fp;
}

char read_command(char* libraryName)
{
    char command;
    printf("%s Command: ",libraryName);
    scanf("%s",&command);
    if(tolower(command)!='i'||tolower(command)!='p'||tolower(command)!='d'||tolower(command)!='l'||tolower(command)!='q')
    command='n';
    //n is for incorrect command
    return command;
        
}

int evaluate_command(char command)
{
    if(tolower(command)=='i');
    else if(tolower(command)=='p');
    else if(tolower(command)=='d');
    else if(tolower(command)=='l');
    else if(tolower(command)=='q');
}
load_MusicLibrary(char * fileInputName)
{
    FILE *ifp;
   
    ifp=open_file(fileInputName,"r");
    //while(fscanf(ifp, "%s %d"));
}

store_MusicLibrary()
{
    
}

void store_MusicLibrary(char* libraryName)
{
    
}

int main()
{
    Song songs[NUM_SONG];
    char* libraryName="myMusic";
	
}
