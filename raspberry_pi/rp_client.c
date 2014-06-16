#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12251

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     fd_set rfds;
     int retval;
     // if (argc < 2) {
     //     fprintf(stderr,"ERROR, no port provided\n");
     //     exit(1);
     // }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(PORT);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,10);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     while(1){
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(newsockfd, &rfds);
        retval = select(newsockfd+1, &rfds,NULL,NULL,NULL);
        if(retval==-1){
            printf("ERROR\n");
        }
        if(FD_ISSET(newsockfd,&rfds)){
            bzero(buffer,256);
            n=read(newsockfd,buffer,255);
            if (n < 0) error("ERROR reading from socket");
            printf("%s",buffer);
        }
        if(FD_ISSET(STDIN_FILENO,&rfds)){
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            n = write(newsockfd,buffer,sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
        }
     }
     close(newsockfd);
     close(sockfd);
     return 0; 
}