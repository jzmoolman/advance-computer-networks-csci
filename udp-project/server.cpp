#include <iostream>
#include <sys/socket.h> /* socket AF_INET SOCK_DGRAM  */
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>  /* close */


#define PACKET_SIZE_ZCP 1000 /* packet size */

struct client_s {
    struct sockaddr_in addr;
    struct client_s *next;
};

struct client_s *clients = NULL;

struct client_s *new_client( struct sockaddr_in addr ) {
    struct client_s *client = (struct client_s*)malloc(sizeof(struct client_s));
    memcpy(&client->addr, &addr, sizeof(struct sockaddr_in) );
    client->next = NULL;
    return client;
}

struct client_s *lookup_client(struct sockaddr_in addr) {
    struct client_s *client = clients;
    for ( ;client != NULL; client = client->next ) {
        if (client->addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
            return client;
        }
    }
    
    client  = clients;
    clients = new_client(addr);
    clients->next = client;
    return clients;
}

void print_addr(struct sockaddr_in addr){

    std::cout << "Family: " <<  ( addr.sin_family == AF_INET ? "AF_INET":"UNKNOWN" )  
       << " IP: " << inet_ntoa(addr.sin_addr) 
       << " Port: " << ntohs(addr.sin_port) << std::endl; 
}

void print_clients() {

    struct client_s *client = clients;
    for ( ;client != NULL; client = client->next ) {
         print_addr(client->addr);
    }
}



int main () {

    char buffer[PACKET_SIZE_ZCP] = {0};

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
    
    while (1) {
        if ( recvfrom(s, buffer, PACKET_SIZE_ZCP, 0, (struct sockaddr*)&client, &client_address_size) < 0 ) {
            perror("recvfrom()");
            exit(4);
         };
        lookup_client(client);
        print_clients();
    }


  
    // std::cout << "Received message: " << buffer << std::endl;
    // std::cout << "Family: " <<  ( client.sin_family == AF_INET ? "AF_INET":"UNKNOWN" )  
    //    << " Client IP: " << inet_ntoa(client.sin_addr) 
    //    << " Port: " << ntohs(client.sin_port) << std::endl; 
    
    //  put packet is queue per source ip
    

    
    
    
        
     
    
    
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
