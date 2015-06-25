/*
Andres Rene Dominguez
CS 344 Operating Systems
Program 4 - One Time Pad (OTP)
File Name: otp_dec_d.c
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <fcntl.h>

// function prototype for my decoding process
void dostuff(int); 
// function for my error messages
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     // signal call to handle zombies
     signal(SIGCHLD,SIG_IGN);
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     // check to see if a port was provided 
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     // create a socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);  
     // error checking on socket
     if (sockfd < 0) 
        error("ERROR opening socket");
        
     // set port number and address information     
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     //error check for binding
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     // listen call to wait for the client to connect          
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     // loop to continue running so the user can keep adding port calls
     while (1) {
         // create new socket   
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
               
         if (newsockfd < 0) 
             error("ERROR on accept");
          
         // fork call to create a new instance    
         pid = fork();
         
         if (pid < 0)
             error("ERROR on fork");
             
         if (pid == 0)  {
             // close the old socket since we are using the new one    
             close(sockfd);
             // function call for my encoding 
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } 
     
     close(sockfd);
     return 0;
}

// my function to decode the information
// there is a seperate instance of it every time we fork
// and it handles all of the communication from this point on
void dostuff (int sock)
{
    int n;
    char socket_buffer[10000];
    bzero(socket_buffer,10000);
    
    // get the plaintext file from the server
    n = read(sock,socket_buffer,10000);
    
    if (n < 0) 
    error("ERROR reading from socket");
    
    // write some information back to the other program
    n = write(sock, "got the message", 15);

    if (n < 0) 
    error("ERROR writing to socket");
    
    // copy information from the buffer to a new variable
    char file[10000];
    strcpy(file, socket_buffer);
    
	int fd;	
	char buffer[512];
	int size;
	int nread;
	int i;
	int j;
	// list of acceptable input for decoding a file
	char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int cipher_codex[512];
	int plaintext_codex[512];
	int badCharactersFlag = 0;

    // call to open file for reading only
	fd = open(file, O_RDONLY);
	
	// check to make sure file opened successfuly 
	if(fd == -1)
	{
		fprintf(stderr, "Could not open %s\n", file);
		exit(1);
	}
	
	bzero(buffer, 512);
	nread = read(fd, buffer, 512);

	size = strlen(buffer);
	
    // check for bad input inside the file	
	for(i = 0; i < size-1; i++)
	{
		for(j = 0; j < 27; j++)
		{
			if(buffer[i] == alphabet[j])
			{
	            badCharactersFlag = 1;
			}
		}
	}
	
	if(badCharactersFlag == 0)
	{
         fprintf(stderr, "ERROR: File contains bad characters.\n");
         exit(2);
    }

    // create numerical version of key
	for(i = 0; i < size-1; i++)
	{
		for(j = 0; j < 27; j++)
		{
			if(buffer[i] == alphabet[j])
			{
				cipher_codex[i] = j;
			}
		}
	}

	close(fd);

    // get the plaintext file from the server
    bzero(socket_buffer,10000);
    n = read(sock,socket_buffer,10000);
    
    if (n < 0) 
    error("ERROR reading from socket");
    
    char file2[10000];
    strcpy(file2, socket_buffer);

	//char file2[] = "plaintext1";
	int fd2;
	int codedMessage[512];
	char buffer2[512];
	int size2;
	int nread2;

    // open the next file that was received from the client
	fd2 = open(file2, O_RDONLY);

    // make sure it was opened properly 
	if(fd2 == -1)
	{
		fprintf(stderr, "Could Not open %s\n", file2);
	}

	bzero(buffer2, 512);
	nread2 = read(fd2, buffer2, 512);

	size2 = strlen(buffer2);
	
	// check to make sure the key that was given is large enough
	if(size < size2)
    {
        fprintf(stderr, "ERROR: Your key is too short.\n");
	    exit(1);
    } 

    // create numerical version of plain text
	for(i = 0; i < size2-1; i++)
	{
		for(j = 0; j < 27; j++)
		{
			if(buffer2[i] == alphabet[j])
			{
				plaintext_codex[i] = j;
			}
		}
		
	}

    // create numerical version of decoded message using modular math
	for(i = 0; i < size2-1; i++)
	{
		if((codedMessage[i] = (plaintext_codex[i] - cipher_codex[i]) % 27) < 0)
		{
			codedMessage[i] = (codedMessage[i] + 27) % 27;
		}
	}

	char codedMessageText[512];

    // create alpha version of decoded message
	for(i = 0; i < size2-1; i++)
	{
		for(j = 0; j < 27; j++)
		{
			if(codedMessage[i] == j)
			{
				codedMessageText[i] = alphabet[j];
			}
		}
	}
	
	// write the decoded message back to the client
	n = write(sock, codedMessageText, strlen(codedMessageText));

    if (n < 0) 
    error("ERROR writing to socket");
	
    close(fd2);   
}
