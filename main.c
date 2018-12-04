//main.cpp

//sudo apt install libssl-dev
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blockchain.h"

int main(int argc, char **argv)
{
	char id[] = "7kzf5fl8dX7lsUfUDSvCcBeAoUVZyfWefr02SzYVkXWjsgDWCRJgHeCAoCkUqRuLK0N5fQLG7fqimZwBLAL40hYCaFwNbjmB1KlNPtm8pwQDSehXzgAPKFPaqPR8KFJDTLcxzSwseZ2fsBHbzZgpud909z2yA5u8kkM9HEAYB7Au7YzMAl9WWAdRkK0ESZwF70RGBdNkOASOx2okb29SVnNDOqG7t7B5LqveNfRtJRqkcHYNlUnsymES3DGnfi6p";
	char license[] = "VrGNpz87DHaYlp97cWS7JFpxlRNruIw9xPGWRfrzHznzwDzw0YB8d4SiEzzI3ewwbGSVUk6aUu66E4olapwvmHuBeg6kBItcuEljq6kFPEuykvaQ0BjmaXCVDwOAkdaK";

	//chdir("cd ~/.blockchain");
	if (argc < 3)
	{
		exit(1);	
	}
	char *service = argv[1];
	const int numHosts = argc - 2;
	char *hosts[numHosts];
	int i;
	for (i = 0; i < numHosts; i++)
	{
		hosts[i] = argv[i + 2];
	}
	
	
	(void) signal(SIGCHLD, reaper);
	int pid = fork();
	
	if (pid > 0)
	{
		//fprintf(stderr, "Main Parent: My PID: %d, Parent PID: %d\n", getpid(), getppid());
		//parent
		(void) server(service);
		exit(0);
	}
	else if (pid == 0)
	{
		(void) client(hosts, service, numHosts);
		exit(0);
 	}
	else
	{
		
		exit(EXIT_FAILURE);
	}
	return 0;
}
