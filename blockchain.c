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
	int msock;
	int ssock;
	int alen;
	struct sockaddr_in fsin;
	
	// listen for transaction
	msock = passiveTCP(service, QLEN);
	// listen for block
	// listen for chain
	(void) signal(SIGCHLD, reaper);
	while (1)
	{
		//fprintf(stderr, "Server Parent: My PID: %d, Parent PID: %d\n", getpid(), getppid());
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
		if (ssock < 0) 
		{
			if (errno == EINTR)   //system call was interrupted permaturely with a signal before it was able to complete
				continue;
			exit(EXIT_FAILURE);
		}
		switch (fork()) 
		{
			case 0:		/* child */
				(void) close(msock);
				//fprintf(stderr, "Server Child: My PID: %d, Parent PID: %d\n", getpid(), getppid());
				//determine if block or transaction
				char buffer;
				int cc;
				cc = read(ssock, &buffer, sizeof(buffer));
				if (cc < 0)
				{
					exit(1);
				}
				if (buffer == 'b')
				{
					fprintf(stderr, "Recieving Block\n");
					recieveBlock(ssock);
					fprintf(stderr, "Recieved Block\n");
				}
				else if (buffer == 't')
				{
					fprintf(stderr, "Recieving Transaction\n");
					recieveTransaction(ssock);	
					fprintf(stderr, "Recieved Transaction\n");
				}
				exit(0);
			default:	/* parent */
				//fprintf(stderr, "Server fork Parent: My PID: %d, Parent PID: %d\n", getpid(), getppid());
				(void) close(ssock);
				pause();
				break;
			case -1:
				exit(1);
		}
	}
}

int blockValidate(Block block)
{
	return 1;
}

int transactionValidate(Transaction trans)
{
	return 1;
}

Block createBlock(int num, char hash[65])
{
	char title[128];
	sprintf(title, "%d", num);
	Block block = {
		"o",
		"o",
		num,
		time(0)
	};
	strcpy(block.prevHash, hash);
	strcpy(block.blockTitle, title);
	return block;
}

Transaction createTransaction(int blocknum, int transnum, char hash[65], char id[257], char key[129])
{
	Transaction trans = {
		transnum,
		blocknum,
		"o",
		"o",
		"o",
		time(0)
	};
	strcpy(trans.prevHash, hash);
	strcpy(trans.ownerKey, id);
	strcpy(trans.license, key);
	return trans;
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

void transmitBlock(Block block, char* host, int s)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
	char validMssg[]="Block Valid\n";
	int cc;
	char header = 'b';

	if (write(s, &header, sizeof(header)) < 0)
	{
		exit(1);
	}
	
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

void broadcastBlock(Block block, char** hosts, char *service, int numHosts)
{
	int i, s;
	for (i = 0; i < numHosts; i++)
	{
		char *host = hosts[i];
		s = connectTCP(host, service);
		transmitBlock(block, host, s);
	}
}

void saveBlockToFile(Block block)
{
	FILE *file;
	file = fopen(block.blockTitle, "w");
	//fprintf(stderr, "Block at Save: %s, PID: %d, Parent PID: %d\n", block.blockTitle, getpid(), getppid());
	//printf("Attempting to save block: %s", block.blockTitle);
	if (file == NULL)
	{
		fprintf(stderr, "\nError opening file1\n"); 
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
		fprintf(stderr, "\nError opening file2\n"); 
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
	char validMssg[]="Transaction Valid\n";
	int cc;
	
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

void transmitTransaction(Transaction trans, char *host, int s)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
	char validMssg[]="Transaction Valid\n";
	int cc;
	char header = 'b';
	fprintf(stderr, "\nSending Header\n");
	if (write(s, &header, sizeof(header)) < 0)
	{
		exit(1);
	}
	fprintf(stderr, "\Sending Transaction\n");
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

void broadcastTransaction(Transaction trans, char **hosts, char *service, int numHosts)
{
	int i, s;
	for (i = 0; i < numHosts; i++)
	{
		char *host = hosts[i];
		s = connectTCP(host, service);
		transmitTransaction(trans, host, s);
	}
}

void saveTransactionToFile(Transaction trans)
{
	FILE *file;
	char s[128];
	sprintf(s, "%d", trans.transactionCount);
	file = fopen(s, "w");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file3\n"); 
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
		fprintf(stderr, "\nError opening file4\n"); 
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
}

void reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
