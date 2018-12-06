CXX=gcc
#INCS= -I. -I/usr/include # to use HPUX with gcc
OBJECTS= blockchain.c connectTCP.c connectsock.c passivesock.c passiveTCP.c errexit.c
OPT = -lssl -lcrypto

.SUFFIXES: .c

all: blockchain

blockchain: main.c $(OBJECTS)
	$(CXX) main.c $(OBJECTS) $(OPT) \
	-o blockchain

clean:
	rm -f *.o *.b *.t core *~ blockchain
