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
#include "string.h"
#include "ftp.h"

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, 
        s,
        tmp,
        j,
        type,
        endFtp,
        debug,
        anonymous,
        connected;
    size_t len;
    ssize_t nread;
    char    buf[BUF_SIZE],
            bufC[1024], 
            trash[120],
            host[1024],
            login[MAX_LOGIN],
            password[MAX_PASSWORD],
            port[20];

    // check the number of args on command line
    if(argc != 1)
    {
        printf("USAGE: ./client \n");
        exit(-1);
    }
    
    endFtp      = 1;
    connected   = 0;
    debug       = 0;
    anonymous   = 0;

    memset(host,'\0',1024);
    memset(port,'\0',20);
    
    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;          /* Any protocol */

    printf("ftp> ");
    
    while(endFtp)
    {   
        if(fgets(bufC, 1024, stdin) == NULL)
            continue;

        type = command(bufC);
        
        if(type == CMD_OPEN)
        {        
            sscanf(bufC, "%s %s %s\n",trash,host,port);

            s = getaddrinfo(host,port,&hints, &result);
            
            if (s != 0) 
            {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }

            /* 
              getaddrinfo() returns a list of address structures.
              Try each address until we successfully connect(2).
              If socket(2) (or connect(2)) fails, we (close the socket
              and) try the next address. 
            */

            for (rp = result; rp != NULL; rp = rp->ai_next) 
            {
                sfd = socket(rp->ai_family, rp->ai_socktype,
                            rp->ai_protocol);
               if (sfd == -1)
                   continue;

               if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
                   break;                  /* Success */

               close(sfd);
            }

            if (rp == NULL) {               /* No address succeeded */
               fprintf(stderr, "Could not connect\n");
               continue;
            }
            else
            {
                /* authentification */
                /* login */
                printf("Name (localhost:anonymous): ");
                fflush(stdout);
                scanf("%s", login);
                while ((tmp = getchar()) != '\n' && tmp != EOF); /* display login */
                if (strcmp(buf, "anonymous") == 0)
                {
                    anonymous = 1;
                }
                sprintf(buf, "USER %s\r\n", login);
                if (send(sfd, buf, strlen(buf), 0) == -1) 
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }
	            memset(buf, '\0', BUF_SIZE);
                receiveFServ(sfd, buf);
                fflush(stdout);
                /* end login */

                /* password */
                printf("Password: ");
                fflush(stdout);
                if (anonymous == 1)
                {
                    if ( (tmp = getchar()) == '\n')
                    {
                        printf("Using binary mode to transfer files.\n");
                        connected = 1;
                    }
                }
                else
                {
                    scanf("%s", password);
                    memset(password, '\0', MAX_PASSWORD);
                    fgets (password, MAX_PASSWORD, stdin);
                    password[strlen(password) - 1] = '\0';
                    sprintf(buf, "PASS %s\r\n", password);
                    if (send(sfd, buf, strlen(buf), 0) == -1) 
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    memset(buf, '\0', BUF_SIZE);
                    receiveFServ(sfd, buf);
                    fflush(stdout);
                    connected = 1;
                }
                /* end password */
                /* end authentification */
                freeaddrinfo(result);           /* No longer needed */
            }
        }
        else if ((type == CMD_EXIT) || (type == CMD_CIAO))
        {
            endFtp = 0 ;
            printf("---> QUIT\n");
            if (debug == 1)
            {
                printf("221 Goodbye.\n");
            }
        }
        else if ((type == CMD_DEBUGON) || (type == CMD_DEBUGOFF))
        {
            if (debug == 0)
            {
                debug = 1;
                printf("Debugging on (debug=1).\n");
            }
            else if (debug == 1)
            {
                debug = 0;
                printf("Debugging on (debug=0).\n");
            }
        }
        else
        {
            if(!connected)
            {
                printf("Not Connected\n");
            }
            else
            { 
                switch(type)
                {
                    case CMD_EXIT :
                    case CMD_CIAO :
                        endFtp = 0;
                        printf("---> QUIT\n");
                        if (debug == 1)
                        {
                            printf("221 Goodbye.\n");
                        }
                        break;
                    case CMD_DIR:
                        break;
                    case CMD_SHOW:
                        break;
                    case CMD_DEBUGON:
                    case CMD_DEBUGOFF:
                        if (debug == 0)
                        {
                            debug = 1;
                            printf("Debugging on (debug=1).\n");
                        }
                        else if (debug == 1)
                        {
                            debug = 0;
                            printf("Debugging on (debug=0).\n");
                        }
                        break;
                    case CMD_GET:
                        break;
                    case CMD_SEND:
                        break;
                    case CMD_REN:
                        break;
                    case CMD_DEL:
                        break;
                    case CMD_CD:
                        break;
                    case CMD_MKD:
                        break;
                    case CMD_RMD:
                        break;
                }
            }
        }
        if (endFtp != 0)
        { 
            printf("ftp> ");
        }
    }

/*
    int count;
    do{
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

    if(sendto(sfd,"USER gov-ftp\n",14,0,
                (struct sockaddr *)rp->ai_addr,rp->ai_addrlen) == -1)
    {
        perror("sendto");
        close(sfd);
        exit(EXIT_FAILURE);
    }

// une demande une reponse en ue ligne si plus que une ligne on a "123-" et a la fin "123 " 
    do{
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

    fflush(stdout);
      printf("end Received\n");*/
    return 0;
}
/*

char buffer[8192]; // or whatever you like, but best to keep it large
int count = 0;
int total = 0;

while ((count = recv(socket, &buffer[total], sizeof buffer - count, 0)) > 0)
{
    total += count;
    // At this point the buffer is valid from 0..total-1, if that's enough then process it and break, otherwise continue
}
if (count == -1)
{
    perror("recv");
}
else if (count == 0)
{
    // EOS on the socket: close it, exit the thread, etc.
}

https://stackoverflow.com/questions/27205810/how-recv-function-works-when-looping

*/