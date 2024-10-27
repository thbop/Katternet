OS := $(shell uname)

all : client clean

ifeq ($(OS), Windows_NT)
client : client.o lib/sha-256.o
	cc -o client.exe client.o lib/sha-256.o -lWs2_32

else ifeq ($(OS), Linux)
client : client.o lib/sha-256.o
	cc -o clientclient.o lib/sha-256.o -lWs2_32

endif

clean :
	rm client.o
	rm lib/sha-256.o