//Pranoto, Timothy: 38964311
//Wongso, Valencia: 80147413

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include "csapp.h"

#define BUFFER_SIZE 1024

bool validate_port(int argc, char** argv);
void run_proxy(int clientfd, FILE* log_file);
void read_uri(char* url, char* host, char* port, char* path);
void write_log(FILE* log_file, char* ip, char* uri, int bytes);

int main(int argc, char** argv) {
  bool check = validate_port(argc, argv); 
  char* port = argv[1]; 
  int listenfd, clientfd; 
  socklen_t client_len;
  struct sockaddr_in client_addr;
  FILE* log_file = NULL;

  if(check == false) {
    exit(1);
  }

  log_file = fopen("proxy.log", "a");
  if(!log_file) {
    puts("Unable to open the log file.");
  }

  listenfd = Open_listenfd(port);
  while(1) {
    client_len = sizeof(client_addr);
    clientfd = Accept(listenfd, (SA*) &client_addr, &client_len);
    run_proxy(clientfd, log_file);
  }

  fclose(log_file);
  return 0;
}

bool validate_port(int argc, char** argv) {
  FILE* servicesFile = NULL;
  int port = -1;
  char serviceName[255] = {'\0'}; 
  char serviceType[255] = {'\0'};
  int servicePort = 0;

  if(argc != 2) {
    fprintf(stderr, "Invalid arguments\n");
    return false;
  }

  port = atoi(argv[1]);

  if(port < 1024 || port > 65536) {
    fprintf(stderr, "Port range > 1024 and < 65536\n");
    return false;
  }

  servicesFile = fopen("/etc/services", "r");
  if(!servicesFile) {
    fprintf(stderr, "Unable to read /etc/services\n");
    return false;
  }

  while(fscanf(servicesFile, "%s %d/%s\n", serviceName, &servicePort, serviceType) != EOF) {
    if(servicePort == port) {
      fprintf(stderr, "Port is used: %s\n", serviceName);
      fclose(servicesFile);
      return false;
    }
  }

  fclose(servicesFile);
  return true;
}

void run_proxy(int clientfd, FILE* log_file) {
  rio_t client_rio; 
  char buffer[BUFFER_SIZE] = {'\0'};
  char method[BUFFER_SIZE] = {'\0'};
  char uri[BUFFER_SIZE] = {'\0'};
  char version[BUFFER_SIZE] = {'\0'};
  char host[BUFFER_SIZE] = {'\0'};
  char port[BUFFER_SIZE] = {'\0'};
  char path[BUFFER_SIZE] = {'\0'};

  struct addrinfo hints, *res;
  int socketfd;
  int response_len = 0;
  int total_response_len = 0;
  int bytes_read;

  Rio_readinitb(&client_rio, clientfd);
  Rio_readlineb(&client_rio, buffer, BUFFER_SIZE); 
  sscanf(buffer, "%s %s %s", method, uri, version);

  if(strcmp(method, "GET") != 0) {
    return;
  }

  read_uri(uri, host, port, path);

  memset(&hints, '\0', sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(host, port, &hints, &res);

  if(!res) {
    return;
  }

  socketfd = Socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd != -1) {
    Connect(socketfd, res->ai_addr, res->ai_addrlen);
  } else {
    puts("Failed creating socket.");
    freeaddrinfo(res);
    return;
  }

  Rio_writen(socketfd, buffer, strlen(buffer));
  while(true) {
    bytes_read = Rio_readlineb(&client_rio, buffer, BUFFER_SIZE);
    Rio_writen(socketfd, buffer, bytes_read);
    if(strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0 || bytes_read == 0) {
      break;
    }
  }

  while((response_len = recv(socketfd, buffer, BUFFER_SIZE, 0)) > 0) {
    total_response_len += response_len;
    Rio_writen(clientfd, buffer, response_len);
  }

  write_log(log_file, inet_ntoa(((struct sockaddr_in*) res->ai_addr)->sin_addr), uri, total_response_len);

  freeaddrinfo(res);
  Close(socketfd);
  Close(clientfd);
}

void read_uri(char* uri, char* host, char* port, char* path) {
  int offset;
  int port_num;
  int slashPos = 0;
  int colonPos = 0;
  char* slashPtr = NULL;
  char* colonPtr = NULL;

  if(strstr(uri, "https://") != NULL) {
    sscanf(uri, "https://%[^/]", host);
    offset = 8;
  } else if(strstr(uri, "http://") != NULL) {
    sscanf(uri, "http://%[^/]", host);
    offset = 7;
  } else {
    sscanf(uri, "%[^/]", host);
    offset = 0;
  }

  slashPtr = strchr(uri + offset, '/');
  colonPtr = strchr(uri + offset, ':');

  slashPos = slashPtr != NULL ? slashPtr - uri : 0;
  colonPos = colonPtr != NULL ? colonPtr - uri : 0;

  if(colonPos != 0){
    memset(host + colonPos - offset, '\0', BUFFER_SIZE - colonPos);
    sscanf(uri + colonPos, ":%d", &port_num);
    sprintf(port, "%d", port_num);
    sscanf(uri + colonPos + 1 + strlen(port), "%s", path);
  }else{
    memset(host + slashPos - offset, '\0', BUFFER_SIZE - slashPos);
    sprintf(port, "80");
    sscanf(uri + slashPos, "%s", path);
  }
}

void write_log(FILE* log_file, char* ip, char* uri, int bytes) {
  char timebuf[BUFFER_SIZE] = {'\0'};
  char logbuf[BUFFER_SIZE] = {'\0'};
  time_t raw;
  struct tm* parsed_time;

  time(&raw);
  parsed_time = localtime(&raw);
  strftime(timebuf, BUFFER_SIZE, "%a %d %b %Y %H:%M:%S %Z", parsed_time);
  sprintf(logbuf, "%s: %s %s %d\n",timebuf,ip,uri,bytes );
  fprintf(log_file, "%s", logbuf);
  fflush(log_file);
}
