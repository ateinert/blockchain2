//blockchain.h

typedef struct {	
	char prevHash[65];
	char blockTitle[128];
	long long int blockNumber;
	time_t creationTime;
} Block;

typedef struct {
	int transactionCount;
	int forSale;
	int parentBlock;
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

void server(char*);					
void client(char** hosts, char *service, const int numHosts);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

void updateWallet();
int loadWallet();

void sell(Transaction trans); 			// client: sends a transaction for sale to other nodes, gets added to list of buyable stuff;
void saleIntent(Transaction); 			// server: recieves intent to sell
void seeForSale(); 						// all: loads the for sale list to UI
void buy(Transaction trans, char*host); // client: sends intent to buy to owner host, host recieves this, then generates and buys new block
void transmitSold(); 					// server: recieves intent to buy and generates and sells new transaction in the owners name.

void updateBlockCount(int);
int loadBlockCount();
Block createBlock(int, char[65]);
Block recieveBlock(int);
void broadcastBlock(Block block, int connections[], int numHosts);
void transmitBlock(Block block, int s);
void saveBlockToFile(Block);
Block loadBlockFromFile(char*);
int blockValidate(Block);

void updateTransactionCount(int);
int loadTransactionCount();
Transaction createTransaction(int, int, int, char[65], char[257], char[129]);
Transaction recieveTransaction(int);
void broadcastTransaction(Transaction trans, int connections[], int numHosts);
void tranmitTransaction(Transaction trans, int s);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(char*);
int transactionValidate(Transaction);
void buyTransaction(char *, int);

void sha256(char *string, char outputBuffer[65]);
int sha256_file(char *path, char outputBuffer[65]);

void reaper(int);

