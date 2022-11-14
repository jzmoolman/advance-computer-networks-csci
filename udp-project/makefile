CC=clang

all: rm client server 
	echo  rebuilding

client: client.cpp zcp.cpp timer.cpp
	echo $@
	$(CC) -o $@ -lstdc++ client.cpp  zcp.cpp timer.cpp

server: server.cpp zcp.cpp timer.cpp
	$(CC) -o $@ -lstdc++ $< zcp.cpp timer.cpp
	
timer: timer.cpp
	$(CC) -o $@ -lstdc++ $<
	
rm:
	rm client server