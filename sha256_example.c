/*
 * sha256_example.c
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
#include <errno.h>
#include <openssl/sha.h>

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

int main(int argc, char **argv)
{
    char str[128];
    char buffer[65];

    if (argc == 1)
    {
        printf("Enter the name of the file you want to see converted to SHA256: ");
        scanf("%s", str);
    }
    else
    {
        strcpy(str, argv[1]);
    }
    sha256_file(str, buffer);
    printf("The hash of the file is: \n%s\n", buffer);
    return 0;
}