//blockchain.h

typedef struct {	
	char prevHash[65];
	char blockTitle[128];
	long long int blockNumber;
	time_t creationTime;
} Block;

typedef struct {
	int transactionCount;
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

void server(char*)

int blockValidate(Block);
int transactionValidate(Transaction);

Block recieveBlock(int);
void broadcastBlock(Block, char**, char*);
void saveBlockToFile(Block);
Block loadBlockFromFile(char*);

Transaction recieveTransaction(int);
void broadcastTransaction(Transaction, char**, char*);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(char*);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

void sha256(char *string, char outputBuffer[65]);
int sha256_file(char *path, char outputBuffer[65]);

void reaper(int);

