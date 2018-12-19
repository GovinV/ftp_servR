#include "ftp.h"

int send(int sockfd)
{

}
void receive(int sockfd) 
{

}

int show(int sockfd, char * file, int debug, int sockdata)
{
    
}

/*
 * 200 PORT command successful. Consider using PASV.
 * ---> LIST
 * 150 Here comes the directory listing.
 * 226 Directory send OK.
*/
int ls(int sockfd, int debug)
{

}
/*
 * ---> QUIT
 * 221 Goodbye.
*/
int exit(int sockfd, int debug)
{

}

/*
 * ---> CWD Documents
 * 250 Directory successfully changed.
*/
int cd(int sockfd, char * file, int debug)
{

}

/*
 * Debugging off (debug=0).
 * Debugging off (debug=1).
*/
int debug(int sockfd, int debug)
{

}

/* 
 * local: <file> remote: <file>
 * ---> TYPE I
 * 200 Switching to Binary mode.
 * ---> PORT 127,0,0,1,223,33
 * 200 PORT command successful. Consider using PASV.
 * ---> RETR <file>
 * 150 Opening BINARY mode data connection for <file> (<byte> bytes).
 * 226 Transfer complete.
 * <byte> bytes received in 0.01 secs (95.3139 kB/s)
*/
int get(int sockfd, char * file, int debug)
{

}