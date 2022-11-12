#include <iostream>
#include <sys/socket.h> /* socket AF_INET SOCK_DGRAM  */
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>  /* close, getpid */
#include <stdio.h>
#include <vector>

#include "zcp.h" 

int main () {
  
    char buffer[1024] = {0};
    int s;
    in_addr_t ip;
    int port;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    std::cout << "Enter server ip address: 127.0.0.1" << std::endl;
    //cin.getline(buffer, 17, '\n'); 
    strcpy(buffer, "127.0.0.1");
    ip = inet_addr(buffer);
    
    std::cout << "Enter server port: 12345" << std::endl;
    //cin.getline(buffer,1024 , '\n'); 
    memset(buffer, 0, 1024);
    strcpy(buffer, "12345");
    port = htons(atoi(buffer));
    if (port == 0 ) {
        perror("port not valid");
        exit(1);
    }
    
    std::cout << "Filename: image_s.png" << std::endl;
    //cin.getline(buffer,1024 , '\n'); 
    memset(buffer, 0, 1024);
    strcpy(buffer, "image_s.png");

    if (( s  = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        // tcperror("socket()");
        perror("socket()");
        exit(2);
    }
    /* client... */ 
    client.sin_family = AF_INET;
    client.sin_port = 0;
    client.sin_addr.s_addr = INADDR_ANY;
    
    if( bind(s, (const struct sockaddr *)&client, sizeof(client)) < 0) {
        perror("bind()");
        exit(3);
    }
    
    
    clientlen = sizeof(client);
    if ( getsockname(s, (struct sockaddr *)&client, &clientlen) < 0 ) {
        perror("bind()");
        exit(4);
    }
    
    std::cout << "Client binding...";
    print_addr(client);

    /* server... */
    server.sin_family = AF_INET;
    server.sin_port = port;
    server.sin_addr.s_addr = ip;
    
    /* Send message */

    struct connection_s *connection = lookup_connection(s, server);
    
    // std::vector< struct packet_s *> list;    
    loadFile(buffer, &(connection->tx_packets));
    openZCP();

    // for ( int i = 0; i < list.size(); i++) {
    // // for ( int i = 0; i < 5; i++) {
    //                         // std::cout << "Dl " << std::endl;
    //     struct packet_s *packet;
    //     packet = list.at(i);
    //     print_packet(packet);
    //     if ( sendto(s, packet,  sizeof(struct data_packet_s), 0, 
    //                     (const struct sockaddr *)&server, sizeof(server)) < 0) {
    //         perror("sendto()");
    //         exit(3);
    //     }
        
        
    //     /* receive ack */
    //     socklen_t serverlen = 0;
    //     memset(buffer, 0, PACKET_SIZE_ZCP);
    //     if ( recvfrom(s, buffer, PACKET_SIZE_ZCP, 0, (struct sockaddr*)&server, &serverlen) < 0 ) {
    //         perror("recvfrom()");
    //         exit(4);
    //      };
    //     // lookup_connection(client);
    //     // std::cout << buffer << std::endl;
    //     // print_connections();
    //     receive_buffer(server, buffer);
    //     // print_packet((struct packet_s *)buffer);
        
    // }
    
    // //print received packetd
    //                                     std::cout << "DB" << std::endl;
    // struct connection_s *connection = lookup_connection(server);
    //                                 std::cout << "packet count " << connection->packets.size() << std::endl;
    // if (connection != NULL) {
    //     print_connection(connection, 1);
    // }
    
    /*  Close socket*/
    close(s);
}