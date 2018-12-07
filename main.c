/*
 * main.c
 * 
 * Blockchain Research
 * Digital License Exchange
 * Aaron Teinert 
 * Dr. Lopamudra Roychoudhuri
 * Honors
 * 
 */

//sudo apt install libssl-dev
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blockchain.h"

int blockCount;
int transactionCount;
char* id;
char* license;

int main(int argc, char **argv)
{
	//initialize the blockCount and transactionCount
	blockCount = 0;
	transactionCount = 0;

	// may load from file instead
	id = "7kzf5fl8dX7lsUfUDSvCcBeAoUVZyfWefr02SzYVkXWjsgDWCRJgHeCAoCkUqRuLK0N5fQLG7fqimZwBLAL40hYCaFwNbjmB1KlNPtm8pwQDSehXzgAPKFPaqPR8KFJDTLcxzSwseZ2fsBHbzZgpud909z2yA5u8kkM9HEAYB7Au7YzMAl9WWAdRkK0ESZwF70RGBdNkOASOx2okb29SVnNDOqG7t7B5LqveNfRtJRqkcHYNlUnsymES3DGnfi6p";
	license = "VrGNpz87DHaYlp97cWS7JFpxlRNruIw9xPGWRfrzHznzwDzw0YB8d4SiEzzI3ewwbGSVUk6aUu66E4olapwvmHuBeg6kBItcuEljq6kFPEuykvaQ0BjmaXCVDwOAkdaK";

	// determine the number of inputs is right
	if (argc < 3)
	{
		fprintf(stderr, "Usage: ./blockchain <service> <host(s)>\n");
		exit(EXIT_FAILURE);	
	}

	// intialize the service;
	char *service = argv[1];
	const int numHosts = argc - 2;

	// get all of the hosts
	char *hosts[numHosts];
	int i;
	for (i = 0; i < numHosts; i++)
	{
		hosts[i] = argv[i + 2];
	}
	
	//reaper
	(void) signal(SIGCHLD, reaper);

	//fork to create client and server
	fflush(stdout);
	int pid = fork();
	if (pid > 0)
	{
		//parent process
		(void) server(service);
		exit(0);
	}
	else if (pid == 0)
	{
		//child process
		(void) client(hosts, service, numHosts);
		exit(0);
 	}
	else
	{
		fprintf(stderr, "Fork Error\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}
