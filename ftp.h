#ifndef STRING_H
#define STRING_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

#define MAX_RECEIVE 256

int send(int sockfd);
void receive(int sockfd);
int show(int sockfd, char * file, int debug, int sockdata);
int ls(int sockfd, int debug);
int exit(int sockfd, int debug);
int cd(int sockfd, char * file, int debug);
int debug(int sockfd, int debug);
int get(int sockfd, char * file, int debug);
int ciao(int sockfd, int debug);

#endif