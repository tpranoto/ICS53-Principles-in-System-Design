#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>

#include "csapp.h"

#define BUFFER_SIZE 2048

// Prototypes
bool validate_port(int argc, char** argv);
void* run_proxy(void*);
void read_uri(char* uri, char* host, char* port, char* path);
bool write_log(char* ip, char* uri, int bytes);

FILE* log_file = NULL;

/*** function main ***
Run the proxy as outlined in lab 4:
https://eee.uci.edu/16s/36680/labs/lab4.pdf
***/
int main(int argc, char** argv) {

  bool validPort = validate_port(argc, argv); // Bool for validating the port.
  char* port = argv[1]; // The port provided to the command line.
  int listenfd, connfd; // Socket file descriptors.
  socklen_t client_len; // Length of client response.
  struct sockaddr_in client_addr; // Address of client.
  int* args = NULL; // Arguments to each thread
  pthread_t tid; // ID of each thread.

  // Make sure the port is valid.
  if(validPort == false) {
    exit(EXIT_FAILURE);
  }

  // Open the log file.
  log_file = fopen("proxy.log", "a");
  if(!log_file) {
    puts("Unable to open the log file.");
    puts("The proxy will run but requests will not be logged.");
  }

  // Begin listening on the specified port.
  listenfd = Open_listenfd(port);
  while(true) {
    client_len = sizeof(client_addr);
    connfd = Accept(listenfd, (SA*) &client_addr, &client_len);

    args = calloc(1, sizeof(int));
    *args = connfd;
    pthread_create(&tid, NULL, run_proxy, args);

  }

  fclose(log_file);
  return 0;

}

/*** function run_proxy ***
Run a proxy for the provided connection.
Input:
# fd : The file descriptor for the active connection.
Returns: void
***/
void* run_proxy(void* args) {

  rio_t client_rio; // The decriptor for the buffer associated with "clientfd". See csapp.c
  char buffer[BUFFER_SIZE] = {'\0'}; // A string for a single line read in from a file.
  char method[BUFFER_SIZE] = {'\0'}; // Method read in from "buffer"
  char uri[BUFFER_SIZE] = {'\0'}; // URI read in from "buffer"
  char version[BUFFER_SIZE] = {'\0'}; // Version read in from "buffer"

  char host[BUFFER_SIZE] = {'\0'}; // The hostname read in from "uri"
  char port[BUFFER_SIZE] = {'\0'}; // The port read in from "uri"
  char path[BUFFER_SIZE] = {'\0'}; // The path read in from "uri"

  // Socket variables
  struct addrinfo hints, *res;
  int socketfd, clientfd;
  int response_len = 0;
  int total_response_len = 0;
  int bytes_read;

  clientfd = *((int*) args);
  Free(args);
  Pthread_detach(pthread_self());

  Rio_readinitb(&client_rio, clientfd); // Associate clientfd with a read buffer.
  Rio_readlineb(&client_rio, buffer, BUFFER_SIZE); // Read in a line from clientfd.
  sscanf(buffer, "%s %s %s", method, uri, version); // Read request data.

  if(strcmp(method, "GET") != 0) {
    Close(clientfd);
    return NULL;
  }

  // Parse the URI data.
  read_uri(uri, host, port, path);

  // Prepare the request data
  memset(&hints, '\0', sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(host, port, &hints, &res);

  if(!res) {
    Close(clientfd);
    return NULL;
  }

  // Send the request
  socketfd = Socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd != -1) {
    Connect(socketfd, res->ai_addr, res->ai_addrlen);
  } else {
    puts("Error creating socket.");
    Close(clientfd);
    freeaddrinfo(res);
    return NULL;
  }

  // Send the request to the server.
  Rio_writen(socketfd, buffer, strlen(buffer));
  while(true) {
    bytes_read = Rio_readlineb(&client_rio, buffer, BUFFER_SIZE);
    Rio_writen(socketfd, buffer, bytes_read);
    if(strcmp(buffer, "\r\n") == 0 || strcmp(buffer, "\n") == 0 || bytes_read == 0) {
      break;
    }
  }

  // Send the response to the client.
  while((response_len = recv(socketfd, buffer, BUFFER_SIZE, 0)) > 0) {
    total_response_len += response_len;
    Rio_writen(clientfd, buffer, response_len);
  }

  while(write_log(inet_ntoa(((struct sockaddr_in*) res->ai_addr)->sin_addr), uri, total_response_len) == false) {
    usleep(5000);
  }

  freeaddrinfo(res);
  Close(socketfd);
  Close(clientfd);
  return NULL;

}

bool validate_port(int argc, char** argv) {

  FILE* servicesFile = NULL; // Pointer to /etc/services.
  int port = -1; // The port number entered into the command line.

  char serviceName[255] = {'\0'}; // The name of a single service.
  char serviceType[255] = {'\0'}; // The type of connection used.
  int servicePort = 0; // The port number used.

  // Validate that the port number exists.
  if(argc != 2) {
    fprintf(stderr, "Invalid arguments\n");
    return false;
  }

  // Read the port as an integer.
  port = atoi(argv[1]);

  // Ensure the port number is in range.
  if(port < 1024 || port > 65536) {
    fprintf(stderr, "Port number out of range 1024 < port number < 65536\n");
    return false;
  }

  // Open /etc/services.
  servicesFile = fopen("/etc/services", "r");
  if(!servicesFile) {
    fprintf(stderr, "Unable to read /etc/services\n");
    return false;
  }

  // Loop through /etc/services and ensure that the port number isn't in use.
  while(fscanf(servicesFile, "%s %d/%s\n", serviceName, &servicePort, serviceType) != EOF) {
    if(servicePort == port) {
      fprintf(stderr, "Port in use by: %s\n", serviceName);
      fclose(servicesFile);
      return false;
    }
  }

  fclose(servicesFile);
  return true;

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

  if(colonPos != 0) {

    memset(host + colonPos - offset, '\0', BUFFER_SIZE - colonPos);

    sscanf(uri + colonPos, ":%d", &port_num);
    sprintf(port, "%d", port_num);

    sscanf(uri + colonPos + 1 + strlen(port), "%s", path);

  } else {

    memset(host + slashPos - offset, '\0', BUFFER_SIZE - slashPos);

    sprintf(port, "80"); // Default to port 80

    sscanf(uri + slashPos, "%s", path);

  }

}

bool write_log(char* ip, char* uri, int bytes) {

  char timebuf[BUFFER_SIZE] = {'\0'};
  char logbuf[BUFFER_SIZE] = {'\0'};
  time_t raw;
  struct tm* parsed_time;
  static bool currently_writing;

  if(currently_writing == true) {
    return false;
  }

  currently_writing = true;

  time(&raw);
  parsed_time = localtime(&raw);
  strftime(timebuf, BUFFER_SIZE, "%a %d %b %Y %H:%M:%S %Z", parsed_time);
  sprintf(logbuf, "%s: %s %s %d\n",
    timebuf,
    ip,
    uri,
    bytes
  );
  if(log_file) {
    fprintf(log_file, "%s", logbuf);
    fflush(log_file);
  }

  currently_writing = false;
  return true;

}