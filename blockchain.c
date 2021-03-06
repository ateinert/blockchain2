/*
 * blockchain.c
 * 
 * Blockchain Research
 * Digital License Exchange
 * Aaron Teinert 
 * Dr. Lopamudra Roychoudhuri
 * Honors Fall 2018
 * 
 */

#include <sys/types.h>
#include <sys/stat.h>
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
#define QLEN 10

extern int blockCount;
extern int transactionCount;
extern char* id;
extern char* license;

Transaction forSale[256];

void client(char** hosts, char *service, const int numHosts)
{
	//child client

	//pause so we can wait for the server to startup
	printf("Enter something to start\n");
	fflush(stdout);
	char rand[128];
	scanf("%s", rand);

	//start the server
	while (1)
	{

		//load the last know number of blocks and transactions
		loadBlockCount();
		loadTransactionCount();

		//print the number of blocks and transactions
		printf("\nBlocks sent: %d\n",	blockCount);
		printf("Transactions sent: %d\n", transactionCount);

		//ask the user what they want to do
		printf("\nType \"publish\" to Send a Block and Transactions\n");
		printf("Type \"buy\" to See a List of Buyable transactions and purchase one\n");

		//connect to the other nodes
		int connections[numHosts];
		int i;
		for (i = 0; i < numHosts; i++)
		{
			connections[i] = connectTCP(hosts[i], service);
		}

		//read in the input
		char s[128];
		scanf("%s", s);

		char buffer[65];

		//publisher
		if (strcmp(s, "publish") == 0)
		{
			loadBlockCount();
			loadTransactionCount();
			strcpy(buffer, "0");
			if (blockCount > 0)
			{
				char str[129];
				sprintf(str, "%d", blockCount - 1);
				sha256_file(strcat(str, ".b"), buffer);
			}
			Block block = createBlock(blockCount, buffer);
			broadcastBlock(block, connections, numHosts);
			int num;
			strcpy(buffer, "0");
			for (i = 0; i < 10; ++i)
			{
				loadTransactionCount();
				Transaction trans = createTransaction(blockCount, transactionCount,1, buffer, id, license);
				broadcastTransaction(trans, connections, numHosts);
				transactionCount++;
				updateTransactionCount(transactionCount);
			}	
			blockCount++;
			updateBlockCount(blockCount);
		}
		//buyer
		else if (strcmp(s, "buy") == 0)
		{
			loadTransactionCount();
			printf("What transaction would you like to buy? ");
			char buy[128];
			scanf("%s", buy);
			Transaction trans = loadTransactionFromFile(buy);
			sha256_file(buy, trans.prevHash);
			trans.transactionCount = transactionCount;
			broadcastTransaction(trans, connections, numHosts);
			transactionCount++;
			updateTransactionCount(transactionCount);
			printf("Purchase Made\n");
		}

		//close all of the connections
		for (i = 0; i < numHosts; i++)
		{
			close(connections[i]);
		}
	}

}

void server(char *service)
{
	int msock;
	int ssock;
	int alen;
	struct sockaddr_in fsin;
	
	// listen for transaction
	msock = passiveTCP(service, QLEN);
	//(void) signal(SIGCHLD, reaper);
	while (1)
	{
		//fprintf(stderr, "Server Parent: My PID: %d\n", getpid());
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
				while (1)
				{
					char buffer = '\0';
					int cc;
					while(cc = read(ssock, &buffer, sizeof(buffer)))
					{
						if (cc < 0)
						{
							exit(1);
						}
						//fprintf(stderr, "Header: %c\n", buffer);
						if (buffer == 'b')
						{
							recieveBlock(ssock);
							loadBlockCount();
							updateBlockCount(++blockCount);
						}
						else if (buffer == 't')
						{
							recieveTransaction(ssock);
							loadTransactionCount();
							updateTransactionCount(++transactionCount);
						}
						else
						{
							exit(0);
						}
					}
				}
				break;
			default:	/* parent */
				(void) close(ssock);
				break;
			case -1:
				exit(1);
		}
	}
}

void updateBlockCount(int count)
{
	FILE *file;
	file = fopen("blockCount", "w");
	fprintf(file, "%d", count);
	blockCount = count;
	fclose(file);
}

int loadBlockCount()
{
	int count = 0;
	FILE *file;
	file = fopen("blockCount", "r");
	if (file != NULL)
	{
		fscanf(file, "%d", &count);
		blockCount = count;
	}
	else	
	{
		file = fopen("blockCount", "w");
		fprintf(file, "%d", 0);
	}
	blockCount = count;
	fclose(file);
	return count;
}

void updateTransactionCount(int count)
{
	FILE *file;
	file = fopen("transactionCount", "w");
	fprintf(file, "%d", count);
	transactionCount = count;
	fclose(file);
}

int loadTransactionCount()
{
	int count = 0;
	FILE *file;
	file = fopen("transactionCount", "r");
	if (file != NULL)
	{
		fscanf(file, "%d", &count);
		transactionCount = count;
	}
	else	
	{
		file = fopen("transactionCount", "w");
		fprintf(file, "%d", 0);
	}
	transactionCount = count;
	fclose(file);
	return count;
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

Transaction createTransaction(int blocknum, int transnum, int toSell, char hash[65], char id[257], char key[129])
{
	Transaction trans = {
		transnum,
		toSell,
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
	char validMssg[]="Block Valid\n";
	int cc;
	
	while (cc = read(fd, &block, sizeof(Block)))
	{
		if (cc < 0)
		{
			exit(1);
		}
		//fprintf(stderr,"Block recieved\n");
		if (blockValidate(block))
		{
			saveBlockToFile(block);
			if (write(fd, validMssg, strlen(validMssg)) < 0)
			{
				exit(1);
			}
			//fprintf(stderr,"Block verified\n");
			return block;
		}
	}
}

void transmitBlock(Block block, int s)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
	char validMssg[]="Block Valid\n";
	int cc;
	char header = 'b';

	if (write(s, &header, sizeof(header)) < 0)
	{
		fprintf(stderr, "Header send failure\n");
		exit(1);
	}
	//fprintf(stderr,"Header Sent\n");
	if (write(s, &block, sizeof(Block)) < 0)
	{
		fprintf(stderr, "Transaction send failure\n");
		exit(1);
	}
	//fprintf(stderr,"Block Sent\n");
	while (cc = read(s, buf, sizeof buf)) 
	{
		if (cc < 0)
		{
			exit(1);
		}
		if(strncmp(buf,validMssg,strlen(validMssg))==0) 
		{
			printf("Block Recieved and Verified\n");
			saveBlockToFile(block);
			break;
		}
	}
}

void broadcastBlock(Block block, int connections[], int numHosts)
{
	int i, s;
	for (i = 0; i < numHosts; i++)
	{
		//fprintf(stderr, "Connecting to host: %s, service: %s\n", host, service);
		transmitBlock(block, connections[i]);
	}
}

void saveBlockToFile(Block block)
{
	FILE *file;
	//char *new = strcat("blocks/", block.blockTitle);
	//mkdir(new, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//char *new2 = strcat(new, block.blockTitle);
	file = fopen(strcat(block.blockTitle, ".b"), "w");
	if (file == NULL)
	{
		//fprintf(stderr, "\nError opening file1\n"); 
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
	//fprintf(stderr,"Block saved to file\n");
}

Block loadBlockFromFile(char *str)
{
	Block block;
	FILE *file;
	file = fopen(str, "r");
	if (file == NULL) 
	{ 
		//fprintf(stderr, "\nError opening file2\n"); 
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
	
	while (cc = read(fd, &trans, sizeof(Transaction)))
	{
		if (cc < 0)
		{
			exit(1);
		}
		//fprintf(stderr,"Transaction Recieved\n");
		if (transactionValidate(trans))
		{
			saveTransactionToFile(trans);
			if (trans.forSale == 1)
			{
				//addToSaleList(trans);
			}
			if (write(fd, validMssg, strlen(validMssg)) < 0)
			{
				exit(1);
			} 
			//fprintf(stderr,"Transaction Verified\n");
			return trans;
		}
	}
}

void transmitTransaction(Transaction trans, int s)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
	char validMssg[]="Transaction Valid\n";
	int cc;
	char header = 't';
	if (write(s, &header, sizeof(header)) < 0)
	{
		fprintf(stderr, "Header send failure\n");
		exit(1);
	}
	//fprintf(stderr,"Header Sent\n");
	if (write(s, &trans, sizeof(Transaction)) < 0)
	{
		fprintf(stderr, "Transaction send failure\n");
		exit(1);
	} 
	//fprintf(stderr,"Transaction Sent\n");
	while (cc = read(s, buf, sizeof buf)) 
	{
		if (cc < 0)
		{
			fprintf(stderr, "Read failure\n");
			exit(1);
		} 
		if(strncmp(buf,validMssg,strlen(validMssg))==0) 
		{
			saveTransactionToFile(trans);
			printf("Transaction Recieved and Verified\n");
			break;
		}
	}
}

void broadcastTransaction(Transaction trans, int connections[], int numHosts)
{
	int i, s;
	for (i = 0; i < numHosts; i++)
	{
		transmitTransaction(trans, connections[i]);
	}
}

void saveTransactionToFile(Transaction trans)
{
	FILE *file;
	char s[129];
	sprintf(s, "%d", trans.transactionCount);
	file = fopen(strcat(s, ".t"), "w");
	if (file == NULL) 
	{ 
		//fprintf(stderr, "\nError opening file3\n"); 
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
	//fprintf(stderr,"Transaction saved to file\n");
}

Transaction loadTransactionFromFile(char *str)
{
	Transaction trans;
	FILE *file;
	file = fopen(str, "r");
	if (file == NULL) 
	{ 
		//fprintf(stderr, "\nError opening file4\n"); 
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
