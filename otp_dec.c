/*
Andres Rene Dominguez
CS 344 Operating Systems
Program 4 - One Time Pad (OTP)
File Name: otp_dec.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include <signal.h>

// function for my error messages
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    // signal call to handle zombies
    signal(SIGCHLD,SIG_IGN);
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[512];

    // make sure that the user entered enough variables
    if (argc < 2) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    // set the port number to the last command entered
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // error checking for socket
    if (sockfd < 0) 
        error("ERROR opening socket");
     
    // set the server to the local host  
    server = gethostbyname("localhost");
    
    // make sure the server exists
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    // set address and port information
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    // connection test
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    { 
        fprintf(stderr, "ERROR: Could not contact port %d.\n", portno);
	    exit(2);
    }

    char *text_file;
    char *my_key;

    text_file = argv[1];
    my_key = argv[2];

    // write the key file to the server
    n = write(sockfd,my_key,strlen(my_key));

    if (n < 0) 
         error("ERROR writing to socket");
         
    bzero(buffer,512);
    n = read(sockfd,buffer,512);
    
    if (n < 0) 
         error("ERROR reading from socket");
         
    // write the plaintext file to the server    
    n = write(sockfd,text_file,strlen(text_file));

    if (n < 0) 
         error("ERROR writing to socket");
    
    // read information from the server 
    bzero(buffer,512);
    n = read(sockfd,buffer,512);
    
    if (n < 0) 
         error("ERROR reading from socket");
    
    printf("%s\n", buffer);
    
    close(sockfd);
    return 0;
}
