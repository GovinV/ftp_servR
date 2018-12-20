#include "ftp.h"


/*


ftp> ren cons.txt kostas.txt
---> RNFR cons.txt
350 Ready for RNTO.
---> RNTO kostas.txt
550 Rename failed.


ftp> cd files
---> CWD files
250 Directory successfully changed.


ftp> cd ..
---> CWD ..
250 Directory successfully changed.


ftp> mkd toto
---> MKD toto
550 Create directory operation failed.



ftp> rmd toto
---> RMD toto
550 Remove directory operation failed.


ftp> del cons.txt
---> DELE cons.txt
550 Delete operation failed.
*/
int cmd_mkd(int sockfd, char * filename, char * buf, int debug)
{
	sprintf(buf, "MKD %s\r\n", filename);
	if (debug == 1)
	{
		printf("---> %s\n", buf);
	}
	send(sockfd,buf,strlen(buf),0);
	receiveFServ(sockfd,buf);
	return 0;
}


int cmd_rmd(int sockfd, char * filename, char * buf, int debug)
{
	sprintf(buf, "RMD %s\r\n", filename);
	if (debug == 1)
	{
		printf("---> %s\n", buf);
	}
	send(sockfd,buf,strlen(buf),0);
	receiveFServ(sockfd,buf);
	return 0;
}

int cmd_del(int sockfd, char * filename, char * buf, int debug)
{
	sprintf(buf, "DELE %s\r\n", filename);
	if (debug == 1)
	{
		printf("---> %s\n", buf);
	}
	send(sockfd,buf,strlen(buf),0);
	receiveFServ(sockfd,buf);
	return 0;
}





int send2Serv(int sockfd)
{

}

int ftp_dataSock(int sfd, int debug, char * buf)
{
	int fd; 
	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1) 
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	getsockname(sfd, (struct sockaddr*)&addr, &addr_size);
	addr.sin_port = ntohs(0);



	if (bind(fd,(const struct sockaddr *)&addr,addr_size)) //get a port for data
	{
		perror("bind");
		close(fd);
		return 1;
	}
	listen(fd, 5);

	getsockname(fd, (struct sockaddr*)&addr, &addr_size);
 	memset(buf, '\0', BUF_SIZE);
	sprintf(buf, "PORT %d,%d,%d,%d,%d,%d\r\n",
				addr.sin_addr.s_addr&0xFF,
				(addr.sin_addr.s_addr&0xFF00)>>8,
				(addr.sin_addr.s_addr&0xFF0000)>>16,
				(addr.sin_addr.s_addr&0xFF000000)>>24,
				 ntohs(addr.sin_port)/256,
				 ntohs(addr.sin_port)%256);
	send(sfd,buf,strlen(buf),0);
	if(debug)
		printf("---> %s",buf);
	receiveFServ(sfd,buf);
	fflush(stdout);
	return fd;

}

void receiveData(int dfd, char *buf)
{
	int count, fd;
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	if((fd = accept(dfd,(struct sockaddr *)&addr,&addr_size))== -1)
    {
        perror("accept");
        close(dfd);
        return;
    }
	do
	{
		memset(buf, '\0', BUF_SIZE);
		count = recv(fd, buf, BUF_SIZE, 0);
		printf("%s",buf);
	} while(count == BUF_SIZE);	
}

void receiveFServ(int sfd, char * buf) 
{
	int count;
	do
	{
		memset(buf, '\0', BUF_SIZE);
		count = recv(sfd, buf, BUF_SIZE, 0);
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

/*
---> PORT 127,0,0,1,202,175
200 PORT command successful. Consider using PASV.
---> RETR constan.txt
150 Opening BINARY mode data connection for constan.txt (10 bytes).
226 Transfer complete.
*/
int cmd_show(int sfd, char * filename, int debug,char * buf)
{
	int dfd = ftp_dataSock(sfd,debug,buf);
	if(dfd == -1)
	{
		printf("ERROR\n");
		return 0;
	}
	sprintf(buf, "RETR %s\r\n",filename);
	if(debug)
		printf("---> %s",buf);
	send(sfd,buf,strlen(buf),0);
	receiveFServ(sfd,buf);
	receiveData(dfd,buf);
	receiveFServ(sfd,buf);
	close(dfd);    
	return 0;
}


/*
 * 200 PORT command successful. Consider using PASV.
 * ---> LIST
 * 150 Here comes the directory listing.
 * 226 Directory send OK.
*/
int cmd_dir(int sfd, int debug,char * buf)
{
	int dfd = ftp_dataSock(sfd,debug,buf);
	if(dfd == -1)
	{
		printf("ERROR\n");
		return 0;
	}
	sprintf(buf, "LIST\r\n");
	if(debug)
		printf("---> %s",buf);
	send(sfd,buf,strlen(buf),0);
	receiveFServ(sfd,buf);
	receiveData(dfd,buf);
	receiveFServ(sfd,buf);
	close(dfd);
	return 0;
}

/*
 * ---> CWD Documents
 * 250 Directory successfully changed.
*/
int cmd_cd(int sockfd, char * directory, char * buf, int debug)
{
	sprintf(buf, "CWD %s\r\n", directory);
	if (debug == 1)
	{
		printf("---> %s\n", buf);
	}
	send(sockfd,buf,strlen(buf),0);
	receiveFServ(sockfd,buf);
	return 0;
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
int cmd_get(int sockfd, char * buf, int debug)
{
	int dfd = ftp_dataSock(sockfd, debug, buf);
	if(dfd == -1)
	{
		printf("ERROR\n");
		return 0;
	}
	sprintf(buf, "GET\r\n");
	if (debug == 1)
	{
		printf("---> %s\n", buf);
	}
	send(sockfd,buf,strlen(buf),0);
	receiveFServ(sockfd,buf);
	receiveData(dfd,buf);
	receiveFServ(sockfd,buf);
	close(dfd);
	return 0;
}


/* ren mkd rmd send */ 