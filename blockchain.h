//blockchain.h

typedef struct {	
	char prevHash[65];
	char blockTitle[128];
	long long int blockNumber;
	time_t creationTime;
} Block;

typedef struct {
	int transactionCount;
	int parentBlock;
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

void server(char*);

Block createBlock(int, char[65]);
Transaction createTransaction(int, int, char[65], char[257], char[129]);

int blockValidate(Block);
int transactionValidate(Transaction);

Block recieveBlock(int);
void broadcastBlock(Block block, char** hosts, char *service, int numHosts);
void transmitBlock(Block block, char* host, int s);
void saveBlockToFile(Block);
Block loadBlockFromFile(char*);

Transaction recieveTransaction(int);
void broadcastTransaction(Transaction trans, char **hosts, char *service, int numHosts);
void tranmitTransaction(Transaction trans, char *host, int s);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(char*);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

void sha256(char *string, char outputBuffer[65]);
int sha256_file(char *path, char outputBuffer[65]);

void reaper(int);

