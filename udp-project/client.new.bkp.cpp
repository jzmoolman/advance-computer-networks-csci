#include <iostream>
#include <sys/socket.h> /* socket AF_INET SOCK_DGRAM  */
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>  /* clode */
int main () {
  
    char buffer[1024] = {0};

    int s, client_address_size;
    socklen_t namelen; 
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
    
    
    
    /*  Close socket*/
    close(s);
  
}
