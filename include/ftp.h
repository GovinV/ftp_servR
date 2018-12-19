#ifndef FTP_H
#define FTP_H

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
#define BUF_SIZE 1024

int send2Serv(int sockfd);
void receiveFServ(int sockfd,char * buf);
int cmd_show(int sockfd, char * file, int debug, int sockdata);
int cmd_ls(int sockfd, int debug);
int cmd_exit(int sockfd, int debug);
int cmd_cd(int sockfd, char * file, int debug);
int cmd_debug(int sockfd, int debug);
int cmd_get(int sockfd, char * file, int debug);
int cmd_ciao(int sockfd, int debug);

#endif