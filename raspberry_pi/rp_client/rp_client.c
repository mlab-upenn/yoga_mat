#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringPi.h>

#define PORT 12251
#define BUTTON1 0
#define BUTTON2 1
#define BUTTON3 2
#define BUTTON4 3
#define CONTROL1 4
#define CONTROL2 5

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void setup(void)
{
  int i ;
  if (geteuid () != 0)
  {
    fprintf (stderr, "Need to be root to run (sudo?)\n") ;
    exit (0) ;
  }
  if(wiringPiSetup() == -1){
    exit(1);
  }
  printf("Setup ...\n");
  fflush(stdout);
  pinMode(CONTROL1,OUTPUT);
  pinMode(CONTROL2,OUTPUT);
  digitalWrite(CONTROL1,0);
  digitalWrite(CONTROL2,0);
  pinMode(BUTTON1,INPUT);
  pinMode(BUTTON2,INPUT);
  pinMode(BUTTON3,INPUT);
  pinMode(BUTTON4,INPUT);
  printf("OK\n");
}

int main(int argc, char *argv[])
{
     setup();
     int sockfd, newsockfd;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     fd_set rfds;
     int retval;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
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
    //exchange files
    // char *fr_name="rp_client.xml";
    // FILE *fr=fopen(fr_name,"a");
    // char revbuf[1000];
    // if(fr==NULL){
    //     printf("File not exist\n");
    // }
    // else{
    //     bzero(revbuf,1000);
    //     int fr_block_sz=0;
    //     while(fr_block_sz=recv(newsockfd,revbuf,1000,0)){
    //         if(fr_block_sz<0){
    //             printf("unable to receive\n");
    //             return 0;
    //         }
    //         int write_sz=fwrite(revbuf,sizeof(char),fr_block_sz,fr);
    //         if(write_sz<fr_block_sz){
    //             printf("cannot write to server\n");
    //         }
    //         else if(fr_block_sz){
    //             break;
    //         }
    //         bzero(revbuf,1000);
    //     }
    // }
    // printf("file exchange successfully\n");
    //control signal
     digitalWrite(CONTROL1,1); 
     while(1){
        
        if(digitalRead(BUTTON2) == HIGH){
            delay(10);
            if(digitalRead(BUTTON2) == HIGH){
                bzero(buffer,256);
                strcpy(buffer,"2\n");
                delay(400);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0){
                    error("ERROR writing to socket");
                }  
            }  
        }
        if(digitalRead(BUTTON3) == HIGH){
            delay(10);
            if(digitalRead(BUTTON3) == HIGH){
                bzero(buffer,256);
                strcpy(buffer,"3\n");
                delay(400);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0){
                    error("ERROR writing to socket");
                }  
            }  
        }
        if(digitalRead(BUTTON4) == HIGH){
            delay(10);
            if(digitalRead(BUTTON4) == HIGH){
                bzero(buffer,256);
                strcpy(buffer,"4\n");
                delay(400);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0){
                    error("ERROR writing to socket");
                }  
            }  
        }
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(newsockfd, &rfds);
        struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=3000;
        retval = select(newsockfd+1, &rfds,NULL,NULL,&tv);
        if(retval==-1){
            printf("ERROR\n");
        }
        if(retval==0){
          if(digitalRead(BUTTON1) == HIGH){
            delay(10);
            if(digitalRead(BUTTON1) == HIGH){
                bzero(buffer,256);
                strcpy(buffer,"1\n");
                delay(400);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0){
                    error("ERROR writing to socket");
                }  
            }  
          }
        }
        if(FD_ISSET(newsockfd,&rfds)){
            bzero(buffer,256);
            n=read(newsockfd,buffer,255);
            if (n < 0) error("ERROR reading from socket");
            if(strcmp(buffer,"a\n")==0){
                digitalWrite(CONTROL1,0);
                digitalWrite(CONTROL2,1);
            }
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