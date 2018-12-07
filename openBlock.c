/*
 * openBlock.c
 * 
 * Blockchain Research
 * Digital License Exchange
 * Aaron Teinert 
 * Dr. Lopamudra Roychoudhuri
 * Honors Fall 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {	
	char prevHash[65];
	char blockTitle[128];
	long long int blockNumber;
	time_t creationTime;
} Block;

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

int main(int argc, char **argv)
{
    Block block;
    char str[128];
    if (argc == 1)
    {
        printf("Enter the name of the block you want to see: ");
        scanf("%s", str);
    }
    else
    {
        strcpy(str, argv[1]);
    }
    block = loadBlockFromFile(str);
    printf("Block Number: %lld\n", block.blockNumber);
    printf("Prev Block Hash: %s\n", block.prevHash);
    printf("Creation time: %s\n", ctime(&block.creationTime));
    return 0;
}