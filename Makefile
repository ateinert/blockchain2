CXX=gcc
#INCS= -I. -I/usr/include # to use HPUX with gcc
OBJECTS= block.o transaction.o sha256.o connectTCP.o connectsock.o passivesock.o passiveTCP.o errexit.o
OPT = -lssl -lcrypto

.SUFFIXES: .c

all: blockchain

blockchain: main.o $(OBJECTS)
	$(CXX) main.o $(OBJECTS) $(OPT) \
	-o blockchain

clean:
	rm -f *.o core *~ blockchain
