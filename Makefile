CXX=gcc
#INCS= -I. -I/usr/include # to use HPUX with gcc
OBJECTS= blockchain.c connectTCP.c connectsock.c passivesock.c passiveTCP.c errexit.c
OPT = -lssl -lcrypto

.SUFFIXES: .c

all: blockchain sha256

blockchain: main.c $(OBJECTS)
	$(CXX) main.c $(OBJECTS) $(OPT) \
	-o blockchain

sha256: sha256_example.c
	$(CXX) sha256_example.c $(OPT) \
	-o sha256

clean:
	rm -f *.o *.b *.t *.out core *~ blockchain sha256
