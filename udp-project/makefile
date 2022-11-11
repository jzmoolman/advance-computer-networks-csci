CC=clang

all: rm client server 
	echo  rebuilding

client: client.cpp zcp.cpp
	echo $@
	$(CC) -o $@ -lstdc++ client.cpp  zcp.cpp

server: server.cpp
	$(CC) -o $@ -lstdc++ $< zcp.cpp
rm:
	rm client server