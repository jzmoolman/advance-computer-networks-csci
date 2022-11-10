#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

using namespace std;

int main () {

    char buffer[1024] = {0};

    int s;
    socklen_t namelen, client_address_size;
    in_addr_t ip;
    int port;
    struct sockaddr_in server, client;

    std::cout << "Enter server port: 12345" << std::endl;
    //cin.getline(buffer,1024 , '\n'); 
    strcpy(buffer, "12345");
    
    port = htons(atoi(buffer));
    if (port == 0 ) {
        perror("port not valid");
        exit(1);
    }

    if (( s  = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        // tcperror("socket()");
        perror("socket()");
        exit(2);
    }
    
    /* Setup up server... */
    server.sin_family = AF_INET;
    server.sin_port = port; /* 0 = use any available port */
    server.sin_addr.s_addr = INADDR_ANY; /* Server's Internet Address */
    
    /* bind socket*/
    if( bind(s, (const struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind()");
        exit(3);
    }
    
    namelen = sizeof(server);
    if (getsockname(s, (struct sockaddr*)&server, &namelen) < 0) {
        perror("bind()");
        exit(3);
    }
    std::cout << "Server ip: " << inet_ntoa(server.sin_addr) << std::endl;
    std::cout << "Server port: " << ntohs(server.sin_port) << std::endl;
  
    client_address_size = sizeof(client);
    if ( recvfrom(s, buffer, 1024, 0, (struct sockaddr*)&client, &client_address_size) < 0 ) {
        perror("recvfrom()");
        exit(4);
     };
  
    std::cout << "Received message " << << " from domain " << << " port " << << " internet\
         adress " << std::endl;
     
    
    
    /*  Close socket*/
    close(s);
  
}
  
 //        memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
 //        bind(udpSocket, (struct socketaddr*)&serverAddr, sizeof(serverAddr));
 //        // bind(udpSocket, &serverAddr, sizeof(serverAddr));
 //       socklen_t addr_size = sizeof(clientAddr); do {
 //            nBytes  = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr*)&clientAddr, &addr_size);
 //            buffer[nBytes] = '\0';
 //            clientIP = inet_ntoa(clientAddr.sin_addr);
 //            cout << clientIP << " says: " << buffer << endl;
 //        } while (strcmp(buffer, "quit") != 0);
 //        close(udpSocket);
 // k      return 0;
