#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

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
     struct hostent *server;
     while(1){
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0) 
            error("ERROR opening socket");
         bzero((char *) &serv_addr, sizeof(serv_addr));
         //portno = atoi(argv[1]);
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(12253);
         if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0) 
                  error("ERROR on binding");
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(PORT);
    
        //printf("send out ping req\n");
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv;
        tv.tv_sec=1;
        tv.tv_usec=0;
        int temp=sockfd;
        retval = select(sockfd+1, &rfds,NULL,NULL,&tv);
        if(retval==-1){
            printf("ERROR\n");
        }
        if(retval==0){
            printf("send out ping request\n");
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            }
            else{
                printf("get ping response\n");
                printf("communication established\n");
                break;
            }
        }   
    }
    printf("start communication\n");
    while(1){    
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(sockfd, &rfds);
        retval = select(sockfd+1, &rfds,NULL,NULL,NULL);
        if(retval==-1){
            printf("ERROR\n");
        }
        if(FD_ISSET(sockfd,&rfds)){
            bzero(buffer,256);
            n=read(sockfd,buffer,255);
            if (n < 0) error("ERROR reading from socket");
            printf("%s",buffer);
            n=write(sockfd,buffer,sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
        }
        if(FD_ISSET(STDIN_FILENO,&rfds)){
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            n = write(sockfd,buffer,sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
        }
    }
    close(newsockfd);
    close(sockfd);
    return 0; 
}