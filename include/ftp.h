#ifndef FTP_H
#define FTP_H

#include "cmdFtp.h"

#define MAX_RECEIVE 256
#define BUF_SIZE 1024
#define MAX_LOGIN 64
#define MAX_PASSWORD 64

int send2Serv(int sockfd);
void receiveFServ(int sockfd,char * buf);
int cmd_show(int sockfd, char * file, int debug, int sockdata);
int cmd_ls(int sockfd, int debug);
int cmd_cd(int sockfd, char * file, int debug);
int cmd_debug(int sockfd, int debug);
int cmd_get(int sockfd, char * file, int debug);

#endif