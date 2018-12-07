/*
 * openTransaction.c
 * 
 * Blockchain Research
 * Digital License Exchange
 * Aaron Teinert 
 * Dr. Lopamudra Roychoudhuri
 * Honors
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	int transactionCount;
	int forSale;
	int parentBlock;
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

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

int main(int argc, char **argv)
{
    Transaction trans;
    char str[128];
    if (argc == 1)
    {
        printf("Enter the name of the transaction you want to see: ");
        scanf("%s", str);
    }
    else
    {
        strcpy(str, argv[1]);
    }
    trans = loadTransactionFromFile(str);
    printf("Transaction Number: %d\n", trans.transactionCount);
    printf("For Sale: %d\n", trans.forSale);
    printf("Parent Block: %d\n", trans.parentBlock);
    printf("Prev Transaction Hash: %s\n", trans.prevHash);
    printf("Owner Key: %s\n", trans.ownerKey);
    printf("License: %s\n", trans.license);
    printf("Creation time: %s\n", ctime(&trans.creationTime));
    return 0;
}