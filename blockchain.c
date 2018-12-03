//blockchain.cpp
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <openssl/sha.h>
#include "blockchain.h"

#define LINELEN 128
#define BLOCK_PORT "2000"
#define TRANSACTION_PORT "2001"
#define QLEN 5

void server(char *service)
{
	int msock;;
	int ssock;
	int alen;
	sockaddr_in fsin;
	
	// listen for transaction
	msock = passiveTCP(service, QLEN);
	// listen for block
	// listen for chain
	(void) signal(SIGCHLD, reaper);
	while (1)
	{
		alen = sizeof(fsin);
		ssock = accept(msock1, (sockaddr *)&fsin, (socklen_t *)&alen);
		if (ssock < 0) 
		{
			if (errno == EINTR)   //system call was interrupted permaturely with a signal before it was able to complete
				continue;
			cerr << "socket failure" << endl;
			exit(EXIT_FAILURE);
		}
		switch (fork()) 
		{
			case 0:		/* child */
				(void) close(msock);
				recieveBlock(ssock);
				int i = 0;
				for (; i < 10; i++)
				{
					recieveTransaction(ssock)	
				}
				exit(0);
			default:	/* parent */
				(void) close(ssock);
				break;
			case -1:
				exit(1);
		}
	}
}

Block recieveBlock(int fd)
{
	Block block;
	char buf[BUFSIZ];
	char endOfFile_Indicator[]="End of file\n";
	char validMssg[]="Block Valid\n";
	int cc;
	
	cc = read(fd, &block, sizeof(Block));
	//printf("recieved: %s, %d\n", buf,cc);
	if (cc < 0)
	{
		exit(1);
	}
	if (blockValidate(block))
	{
		saveBlockToFile(block);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
		{
			exit(1);
		}
		return block;
	}
}

void broadcastBlock(Block block, char** hosts, char *sock, int numHosts)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
   	char endoffile[]="End of file\n";
	int s, i, n, cc;			/* socket descriptor, read count*/
	int outchars, inchars;	/* characters sent and received	*/
	char validMssg[]="Block Valid\n";
	for (i = 0; i < numHosts; i++)
	{
		char *host = hosts[i];
		s = connectTCP(host, sock);
		if (write(s, &block, sizeof(Block)) < 0)
		{
			exit(1);
		}
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
			{
				exit(1);
			}
			if(strncmp(buf,validMssg,strlen(validMssg))==0) 
			{
				//printf("File Recieved and Verified\n");
				saveBlockToFile(block);
				break;
			}
		}
	}
}

void saveBlockToFile(Block block)
{
	FILE *file;
	file = fopen(block.blockTitle, "w");
	//printf("Attempting to save block: %s\n", block.blockTitle);
	if (file == NULL)
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		{
			exit(1);
		} 
	}
	fwrite (&block, sizeof(Block), 1, file); 
	if(fwrite == 0)  
	{
		exit (1);
	}
	fclose(file);
}

Block loadBlockFromFile(char *str)
{
	Block block;
	FILE *file;
	file = fopen(str, "r");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		exit (1); 
	}
	while(fread(&block, sizeof(Block), 1, file));
	// close file 
	fclose(file); 
	return block;
}

Transaction recieveTransaction(int fd)
{
	Transaction trans;
	char buf[BUFSIZ];
	char endOfFile_Indicator[]="End of file\n";
	char validMssg[]="Transaction Valid\n";
	int	cc;
	
	cc = read(fd, &trans, sizeof(Transaction));
	//printf("recieved: %s, %d\n", buf,cc);
	if (cc < 0)
	{
		exit(1);
	} 
	if (transactionValidate(trans))
	{
		saveTransactionToFile(trans);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
		{
			exit(1);
		} 
		return trans;
	}
}

void broadcastTransaction(Transaction trans, char **hosts, char *sock, int numHosts)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
   	char endoffile[]="End of file\n";
	int s, i, n, cc;			/* socket descriptor, read count*/
	int outchars, inchars;	/* characters sent and received	*/
	char validMssg[]="Transaction Valid\n";
	for (i = 0; i < numHosts; i++)
	{
		char *host = hosts[i];
		s = connectTCP(host, sock.c_str());
		if (write(s, &trans, sizeof(Transaction)) < 0)
		{
			exit(1);
		} 
		
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
			{
				exit(1);
			} 
			if(strncmp(buf,validMssg,strlen(validMssg))==0) 
			{
				saveTransactionToFile(trans);
				//printf("File Recieved and Verified\n");
				break;
			}
		}
	}
}

void saveTransactionToFile(Transaction trans)
{
	FILE *file;
	file = fopen(trans.transactionTitle, "w");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		{
			exit(1);
		} 
	}
	fwrite (&trans, sizeof(Transaction), 1, file); 
	if(fwrite == 0)  
	{
		printf("Error writing file\n"); 
		exit (1);
	}
	fclose(file);
}

Transaction loadTransactionFromFile(char *str)
{
	Transaction trans;
	FILE *file;
	file = fopen(str, "r");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		exit (1); 
	}
	while(fread(&trans, sizeof(Transaction), 1, file));
	// close file 
	fclose(file); 
	return trans;
}
 
void sha256(char *string, char outputBuffer[65])
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, string, strlen(string));
	SHA256_Final(hash, &sha256);
	int i = 0;
	for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	outputBuffer[64] = 0;
}

int sha256_file(char *path, char outputBuffer[65])
{
	FILE *file = fopen(path, "rb");
	if(!file) return -534;

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	char *buffer = (char*)malloc(bufSize);
	int bytesRead = 0;
	if(!buffer) return ENOMEM;
	while((bytesRead = fread(buffer, 1, bufSize, file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);
	int i;
	for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	fclose(file);
	free(buffer);
	return 0;

void reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
