CFLAGS=--std=c99 -Wall -pedantic -lm -lcrypto

main: main.c feistel.o
	cc main.c feistel.o $(CFLAGS)

feistel: feistel.h feistel.c
	cc -c feistel.c $(CFLAGS)

cunit: cunit_test.c feistel.o
	cc cunit_test.c feistel.o $(CFLAGS) -lcunit -o test; ./test
