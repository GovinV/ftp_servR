#include "ftp.h"

int send2Serv(int sockfd)
{

}

void receiveFServ(int sfd, char * buf) 
{
	int count;
	do
	{
		count = recv(sfd,buf,BUF_SIZE,0);
		int end = 1;
		char *s = buf;
		while(*s != '\0')
		{
			if(*s == '\r')
			{
				s++;
				if(*s=='\n')
					end = 0;
			}
			if(end)
				printf("%c", *s);
			else
				break;
			s++;
		}
		if(!end)
		{
			printf("\n");
			break;
		}
	} while(count == BUF_SIZE);
}

int cmd_show(int sockfd, char * file, int debug, int sockdata)
{
    
}

/*
 * 200 PORT command successful. Consider using PASV.
 * ---> LIST
 * 150 Here comes the directory listing.
 * 226 Directory send OK.
*/
int cmd_ls(int sockfd, int debug)
{

}

/*
 * ---> CWD Documents
 * 250 Directory successfully changed.
*/
int cmd_cd(int sockfd, char * file, int debug)
{

}

/*
 * Debugging off (debug=0).
 * Debugging off (debug=1).
*/
int cmd_debug(int sockfd, int debug)
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
int cmd_get(int sockfd, char * file, int debug)
{

}

/*
 * ---> QUIT
 * 221 Goodbye.
*/
int cmd_ciao(int sockfd, int debug)
{

}
/*
 * ---> QUIT
 * 221 Goodbye.
*/
int cmd_exit(int sockfd, int debug)
{

}
/* ren mkd rmd send */ 