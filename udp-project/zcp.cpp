#include <iostream>
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */

#include "zcp.h"

struct client_s *clients = NULL;

void print_addr(struct sockaddr_in addr  ) {
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

void print_packet(packet_s *packet){
  std::cout << "packet seq: "  << packet->seq  << " size: " << packet->size << std::endl;
  std::cout << "buffer begin>" << std::endl;
  std::cout << packet->buffer << std::endl;
  std::cout << "<buffer end" << std::endl;
}

struct client_s *new_client( struct sockaddr_in addr ) {
    struct client_s *client = (struct client_s*)malloc(sizeof(struct client_s));
    memset(client, 0, sizeof(struct client_s));
    memcpy(&client->addr, &addr, sizeof(struct sockaddr_in) );
    client->next = NULL;
    return client;
}

struct client_s *lookup_client(struct sockaddr_in addr) {
    struct client_s *client = clients;
    for ( ;client != NULL; client = client->next ) {
        if (client->addr.sin_addr.s_addr == addr.sin_addr.s_addr && 
            client->addr.sin_port == addr.sin_port ) {
            return client;
        }
    }
    
    client  = clients;
    clients = new_client(addr);
    clients->next = client;
    return clients;
}

void new_client_buffer(struct sockaddr_in addr, char* buffer) {
    struct client_s *client = lookup_client(addr);
    if ( client == NULL) {
        perror("new_client_buffer()");
        exit(1);
    }
    packet_s *packet = (packet_s *)buffer;
    print_packet(packet);
 }

