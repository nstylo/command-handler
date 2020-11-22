CC = gcc

all: requester receiver clean

requester: minus.o plus.o requester.c
	${CC} requester.c plus.o minus.o -o requester

receiver: minus.o plus.o receiver.c
	${CC} receiver.c plus.o minus.o -o receiver

clean:
	rm -rf minus.o plus.o
