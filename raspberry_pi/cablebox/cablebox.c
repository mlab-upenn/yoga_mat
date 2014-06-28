#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>


/*define bind port and communication socket
*To discover the client and cloud service, the server should 
*bind to a port. 
*ORIG the port that server has bind
*PORT the port that server connect to raspberry client
*WEB the port that server has bind 
*WEBPORT the port that server connect to clooud service
*/
#define PORT 12251
#define ORIG 12345
#define WEB 11113
#define WEBPORT 12321


//number sequence
int a[6]={1,3,4,2,1,3};
int b[7]={1,4,3,2,3,1};
int sockfd;
int newsockfd;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/*
thread to connect to the web service

*/
void *connect_web()
{
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     struct sockaddr_in webserv_addr,webcli_addr;
     int n;
     fd_set rfds;
     int retval;
     struct hostent *server;
     while(1){
         newsockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (newsockfd < 0) 
            error("ERROR opening socket");
         bzero((char *) &serv_addr, sizeof(serv_addr));
         //portno = atoi(argv[1]);
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(WEB);
         if (bind(newsockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0){
            error("ERROR on binding");
            close(newsockfd);

         } 
                  
        server = gethostbyname("127.0.0.1");
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(WEBPORT);
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(newsockfd, &rfds);
        struct timeval tv;
        tv.tv_sec=1;
        tv.tv_usec=0;
        int temp=newsockfd;
        retval = select(newsockfd+1, &rfds,NULL,NULL,&tv);
        printf("send out web ping request\n");
            if (connect(newsockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
              close(newsockfd);
            }
            else{
                printf("get web ping response\n");
                printf("communication established\n");
                break;
            }
        sleep(2);
        if(retval==-1){
            printf("ERROR\n");
        }
        // used for timing but work on unix not on raspberry pi. 
        // if(retval==0){
        //     printf("send out ping request\n");
        //     if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        //     }
        //     else{
        //         printf("get ping response\n");
        //         printf("communication established\n");
        //         break;
        //     }
        // }   
    }
    //exchange files
    // char *fs_name="cable_box.xml";
    // char sdbuf[1000];
    // FILE *fs=fopen(fs_name,"r");
    // if(fs==NULL){
    //     printf("file not exist\n");
    // }
    // bzero(sdbuf,1000);
    // int fs_block_sz;
    // while((fs_block_sz=fread(sdbuf,sizeof(char),1000,fs))>0){
    //     if(send(sockfd,sdbuf,fs_block_sz,0)<0){
    //         printf("unable to send file\n");
    //         return 0;
    //     }
    //     bzero(sdbuf,1000);
    // }
    //control signal
    printf("start communication\n");
    printf("new sockfd: %d\n", newsockfd );
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
            if (n < 0) {
                error("ERROR reading from socket");
                return 0;
            }
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
    pthread_exit(NULL);
}

//connect to raspberry client
int main(int argc, char *argv[])
{
     pthread_t thread;
    int rc;
    rc=pthread_create(&thread,NULL,connect_web,NULL);
    static int i=0;
     
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     fd_set rfds;
     int retval;
     struct hostent *server;
     if(argc !=2){
        printf("usage: ./server <host>");
     }
     while(1){
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0) 
            error("ERROR opening socket");
         bzero((char *) &serv_addr, sizeof(serv_addr));
         //portno = atoi(argv[1]);
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(ORIG);
         if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0){
            error("ERROR on binding");
            close(sockfd);

         } 
                  
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
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(sockfd, &rfds);
        struct timeval tv;
        tv.tv_sec=1;
        tv.tv_usec=0;
        int temp=sockfd;
        retval = select(sockfd+1, &rfds,NULL,NULL,&tv);
        printf("send out ping request\n");
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
              close(sockfd);
            }
            else{
                printf("get ping response\n");
                printf("communication established\n");
                break;
            }
        sleep(2);
        if(retval==-1){
            printf("ERROR\n");
        }
        // used for timing but work on unix not on raspberry pi. 
        // if(retval==0){
        //     printf("send out ping request\n");
        //     if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        //     }
        //     else{
        //         printf("get ping response\n");
        //         printf("communication established\n");
        //         break;
        //     }
        // }   
    }
    //exchange files
    // char *fs_name="cable_box.xml";
    // char sdbuf[1000];
    // FILE *fs=fopen(fs_name,"r");
    // if(fs==NULL){
    //     printf("file not exist\n");
    // }
    // bzero(sdbuf,1000);
    // int fs_block_sz;
    // while((fs_block_sz=fread(sdbuf,sizeof(char),1000,fs))>0){
    //     if(send(sockfd,sdbuf,fs_block_sz,0)<0){
    //         printf("unable to send file\n");
    //         return 0;
    //     }
    //     bzero(sdbuf,1000);
    // }
    //control signal
    printf("start communication\n");
    printf("sockfd: %d\n", sockfd );
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
            if (n < 0) {
                error("ERROR reading from socket");
                return 0;
            }
            printf("%d\n",atoi(buffer));
            if(atoi(buffer)==a[i]){
                printf("i is : %d\n", i);
                if(i==5){
                    printf("got it!!!!\n");
                    bzero(buffer,256);
                    strcpy(buffer,"a\n");
                    //n=write(sockfd,buffer,sizeof(buffer));
                    n=write(newsockfd,buffer,sizeof(buffer));
                    if (n < 0) error("ERROR writing to socket");
                }
                i++;
            }
            else{
                i=0;
            }
            n=write(sockfd,buffer,sizeof(buffer));    
        }
        if(FD_ISSET(STDIN_FILENO,&rfds)){
            bzero(buffer,256);
            fgets(buffer,255,stdin);
            n = write(sockfd,buffer,sizeof(buffer));
            if (n < 0) error("ERROR writing to socket");
        }
    }
    close(sockfd);
    pthread_exit(NULL);
    return 0; 
}