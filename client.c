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

#define BUF_SIZE 1024

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;
    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];


    // check the number of args on command line
    if(argc < 3)
    {
      printf("USAGE: ./client \n", argv[0]);
      exit(-1);
    }
    char bufC[1024];
    fgets(buf,1024,stdin);
    
    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;          /* Any protocol */

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
       sfd = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
       if (sfd == -1)
           continue;

       if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
           break;                  /* Success */

       close(sfd);
    }

    printf("Connexion ok\n");

    if (rp == NULL) {               /* No address succeeded */
       fprintf(stderr, "Could not connect\n");
       exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Send remaining command-line arguments as separate
      datagrams, and read responses from server */
    // if(sendto(sfd,"USER gov-ftp\n",14,0,
    //             (struct sockaddr *)rp->ai_addr,rp->ai_addrlen) == -1)
    // {
    //     perror("sendto");
    //     close(sfd);
    //     exit(EXIT_FAILURE);
    // }
   int count = 0;
   char st[1024], t[1024];
  
   
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
      printf("end Received\n");
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