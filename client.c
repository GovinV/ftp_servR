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
#include "cmdFtp.h"
#include "ftp.h"

int main(void)
{

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, 
        s,
        type,
        endFtp,
        debug,
        anonymous,
        connected;

    char    buf[BUF_SIZE],
            bufC[1024], 
            trash[120],
            host[1024],
            login[MAX_LOGIN],
            password[MAX_PASSWORD],
            port[20];

    // check the number of args on command line
    // if(argc != 1)
    // {
    //     printf("USAGE: ./client \n");
    //     exit(-1);
    // }
    
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
        {
            printf("ftp> ");
            continue;
        }
        if(bufC[0] == '\n')
        {
            printf("ftp> ");
            continue;            
        }
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

            if (rp == NULL) 
            {               /* No address succeeded */
               fprintf(stderr, "Could not connect\n");
               printf("ftp> ");
               continue;
            }
            else
            {
                receiveFServ(sfd, buf);
                /* authentification */
                /* login */
                printf("Name (localhost:anonymous): ");
                fflush(stdout);
                scanf("%s", login);
                // if (strcmp(buf, "anonymous") == 0)
                // {
                //     anonymous = 1;
                // }
                sprintf(buf, "USER %s\n", login);
                if (sendto(sfd, buf, strlen(buf),0,
                (struct sockaddr *)rp->ai_addr,rp->ai_addrlen) == -1) 
                {
                    perror("sendto");
                    exit(EXIT_FAILURE);
                }
	            memset(buf, '\0', BUF_SIZE);
                receiveFServ(sfd, buf);

                /* end login */

                /* password */
                printf("Password: ");
                fflush(stdout);
                scanf("%s", password);
                sprintf(buf, "PASS %s\n", password);
                if (sendto(sfd, buf, strlen(buf),0,
                (struct sockaddr *)rp->ai_addr,rp->ai_addrlen) == -1) 
                {
                    perror("sendto");
                    exit(EXIT_FAILURE);
                }
                memset(buf, '\0', BUF_SIZE);
                receiveFServ(sfd, buf);

                /* end password */
                /* end authentification */
                connected = 1;
                freeaddrinfo(result);           /* No longer needed */
                fgets(bufC, 1024, stdin);//get \n
            }
        }
        else if ((type == CMD_EXIT) || (type == CMD_CIAO))
        {
            endFtp = 0 ;
           
            if (debug == 1)
            {
                 printf("---> QUIT\n");
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
                        
                        if (debug == 1)
                        {
                            printf("---> QUIT\n");
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
        if (endFtp)
        { 
            printf("ftp> ");
        }
    }


    return 0;
}