all : client.exe clean

client.exe : client.o lib/sha-256.o
	cc -o client.exe client.o lib/sha-256.o -lWs2_32

clean :
	rm client.o
	rm lib/sha-256.o