/*
Andres Rene Dominguez
CS 344 Operating Systems
Program 4 - One Time Pad (OTP)
File Name: otp_enc_d.c
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char *argv[])
{
	srand(time(0));
	char random_letter;
	int i;

    // a loop that will output random uppercase letters or a space to
    // stdout or into a file if the user uses > command
	for(i = 0; i < atoi(argv[1]); i++)
	{
		random_letter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random() % 27];
		putc(random_letter, stdout);
	}			
  
    // print a new line at the end of the file for reading later
	printf("\n"); 

	return(0);
}

