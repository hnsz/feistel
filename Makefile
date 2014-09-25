CC=cc
FLAGS=--std=c99 -Wall -pedantic -g
CFLAGS=$(FLAGS) -c
OBJS=feistel.o blockcipher.o


a.out: main.c $(OBJS)
	$(CC) main.c $(OBJS) $(FLAGS) -lcrypto

feistel.o: feistel.h feistel.c
	$(CC) $(CFLAGS) feistel.c 

blockcipher.o: blockcipher.h blockcipher.c
	$(CC) $(CFLAGS) blockcipher.c
