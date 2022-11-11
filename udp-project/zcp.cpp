#include <iostream>
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */

#include <vector>

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
}

struct client_s *new_client( struct sockaddr_in addr ) {
    struct client_s *client = (struct client_s*)malloc(sizeof(struct client_s));
    memset(client, 0, sizeof(struct client_s));
    memcpy(&client->addr, &addr, sizeof(struct sockaddr_in) );
    client->next = NULL;
    client->base = 0;
    // client->packets = new std::vector<struct packet_s*>;

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

void new_packet_list(std::vector<struct packet_s * > * list, char* buffer, int size) {
                                   // std::cout << "DB " << std::endl;
    if (list == NULL && buffer == NULL) {
        perror("new_packet_list()");
        exit(999);
    }
                                   // std::cout << "size " << size << std::endl;
    int packet_buffer_size = PACKET_SIZE_ZCP - 2*sizeof(unsigned int);
                                   // std::cout << "packet_buffer_size" << packet_buffer_size << std::endl;
    struct packet_s *packet;
    for (int i = 0; i < size; i++) {
                                   // std::cout << "DB " << i << std::endl;
        int packet_buffer_i = i % packet_buffer_size;
                                   // std::cout << "packet_buffer_i" << packet_buffer_i << std::endl;
                                   // std::cout << "DB " << i << std::endl;
        if ( packet_buffer_i == 0 ) {
            packet = (struct packet_s *)malloc(sizeof(struct packet_s));
            packet->seq = list->size()+1;
            memset(packet->buffer, 0, packet_buffer_size);
            list->push_back(packet);
        }
        packet->buffer[packet_buffer_i] = *(buffer+i);
        packet->size = packet_buffer_i+1;
    }
                                   // std::cout << "DE " << std::endl;
    return;
}

void loadFile(char* filename, std::vector<struct packet_s* > *list) {
    
    FILE *file;
    char *buffer;
    long size;
                            // std::cout << "DE " << std::endl;
    file = fopen(filename, "r");
    if (file == NULL ) {
        perror("loadFile()");
        exit(999);
    }

    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    
    fseek(file, 0L, SEEK_SET);
    
    buffer = (char *)malloc(sizeof(size));
    if (buffer == NULL ) {
        perror("Out of memory");
        exit(1);
    }
    
    fread(buffer, sizeof(char), size, file);
    fclose(file);
    
    new_packet_list(list, buffer, size);
                           // std::cout << "D1 " << list->size() << std::endl;

}