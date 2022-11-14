#include <iostream>
#include <netinet/in.h> /* sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>    /* sleep */

#include <vector>
#include <pthread.h>

#include "zcp.h"
// #include "timer.h"

extern void start_timer(int time);
extern void stop_timer();
extern int timeout_signal;

struct connection_s *connections = NULL;

pthread_t tx_t, rx_t;

void print_addr(struct sockaddr_in addr  ) {
    std::cout << "Family: " <<  ( addr.sin_family == AF_INET ? "AF_INET":"UNKNOWN" )  
       << " IP: " << inet_ntoa(addr.sin_addr) 
       << " Port: " << ntohs(addr.sin_port) << std::endl; 
}

void print_packet(packet_s *packet) {
                                // std::cout << "D0" << std::endl;
    switch (packet->type) {
        case PACKET_ACK: {/* ACK */
            struct ack_packet_s *p = (struct ack_packet_s *)packet;
            std::cout << "ack: "  <<" seq " << p->seq << std::endl;
            break;
        }    
        case PACKET_DATA:{ /* DATA PACKET */
            struct data_packet_s *p = (struct data_packet_s *)packet;
            std::cout << "packet seq: "  << p->seq  << " size: " << p->size << std::endl;
            break;
        }
        default:
            std::cout << "Not implemented " << std::endl;
               
    }
}

void print_connections() {
    struct connection_s *connection = connections;
    for ( ;connection!= NULL; connection = connection->next ) {
         print_addr(connection->addr);
    }
}

void print_connection(struct connection_s  *connection, int summary) {
    print_addr(connection->addr);
    std::cout << "TX packets.." << std::endl;
    for ( int i = 0; i < connection->tx_packets.size(); i++) {
        print_packet(connection->tx_packets.at(i));
    }
    std::cout << "RX packets.." << std::endl;
    for ( int i = 0; i < connection->rx_packets.size(); i++) {
        print_packet(connection->rx_packets.at(i));
    }
}


struct connection_s *new_connection( int type, int fs, struct sockaddr_in addr ) {
    struct connection_s *connection = (struct connection_s*)malloc(sizeof(struct connection_s));
    memset(connection, 0, sizeof(struct connection_s));
    connection->type = type;
    connection->fs = fs;
    memcpy(&connection->addr, &addr, sizeof(struct sockaddr_in) );
    connection->next = NULL;
    connection->tx_base = 0;
    connection->tx_seq = 0;
    // connection->tx_mutex = PTHREAD_MUTEX_INITIALIZER;
    // connection->rx_mutex = PTHREAD_MUTEX_INITIALIZER;
    return connection;
}

struct connection_s *lookup_connection(int type,  int fs, struct sockaddr_in addr) {
    struct connection_s *connection = connections;
    for ( ;connection != NULL; connection = connection->next ) {
        if (connection->addr.sin_addr.s_addr == addr.sin_addr.s_addr && 
            connection->addr.sin_port == addr.sin_port ) {
            return connection;
        }
    }
    
    connection  = connections;
    connections = new_connection(type, fs, addr);
    connections->next = connection;
   
    print_connections();   // print connection when new connetion is seen
    return connections;
}

void receive_buffer(int type, int fs, struct sockaddr_in addr, char *buffer) {
                                   // std::cout << "DB receive_buffer()" << std::endl;
    struct connection_s *connection = lookup_connection(type, fs, addr);
    if ( connection == NULL) {
        perror("new_connection_buffer()");
        exit(1);
    }
    // packet_s *packet = (struct packet_s *)buffer;

    struct packet_s *packet = (struct packet_s *)malloc(PACKET_SIZE_ZCP);
    memcpy(packet, buffer, PACKET_SIZE_ZCP);
    connection->rx_packets.push_back(packet);
        
    print_packet(packet);
                                   // std::cout << "DB receive_buffer() exit" << std::endl;
 }

void append_data_packet_list(std::vector<struct packet_s * > * list, char* buffer, int size) {
                                   // std::cout << "DB " << std::endl;
    if (list == NULL && buffer == NULL) {
        perror("new_packet_list()");
        exit(999);
    }
                                       // std::cout << "size " << size << std::endl;
                                       // std::cout << "packet_buffer_size" << packet_buffer_size << std::endl;
    struct data_packet_s *packet;
    for (int i = 0; i < size; i++) {
                                   // std::cout << "DB " << i << std::endl;
        int packet_buffer_i = i % PACKET_BUFFER_SIZE;
                                       // std::cout << "packet_buffer_i" << packet_buffer_i << std::endl;
                                       // std::cout << "DB " << i << std::endl;
        if ( packet_buffer_i == 0 ) {
            packet = (struct data_packet_s *)malloc(sizeof(struct data_packet_s));
            packet->type = PACKET_DATA;
            packet->seq = list->size()+1;
            memset(packet->buffer, 0, PACKET_BUFFER_SIZE);
            list->push_back((struct packet_s *)packet);
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
    
                           // std::cout << "D1 " << list->size() << std::endl;
    append_data_packet_list(list, buffer, size);
                           // std::cout << "D1 " << list->size() << std::endl;
}

void *tx_exec(void *args) {
    if (connections == NULL) { 
        perror("No connection initialized.");
        exit(1);
    }
    int  tx_ack = 0;
    while (!tx_ack) {
        /* loop through all connections */
        struct connection_s *connection = connections;
        for ( ; connection != NULL; connection = connection->next) {
            if ( timeout_signal ) {
                                                    std::cout << "D0 tx receive timeout signal" << connection->tx_seq << std::endl;
                connection->tx_seq = connection->tx_base; /* resend packets from base */
                                 
            }
            if (connection->tx_packets.size() > connection->tx_seq) {
                pthread_mutex_lock(&connection->tx_mutex);
                print_packet(connection->tx_packets.at(connection->tx_seq));
                if (sendto(connection->fs,
                           connection->tx_packets.at(connection->tx_seq),
                           sizeof(struct data_packet_s),
                           0, 
                           (const struct sockaddr *)&connection->addr,
                           sizeof(connection->addr)) < 0 ) {
                    
                    perror("sendto()");
                    exit(3);                    
                }

                                                    std::cout << "D0 " << connection->tx_seq << std::endl;
                                                    std::cout << "D1 " << connection->tx_base << std::endl;
                if (connection->tx_base == connection->tx_seq) { 
                    start_timer(2);
                }
                connection->tx_seq++;
                pthread_mutex_unlock(&connection->tx_mutex);
            } else {
                                                    // std::cout << "D0 " << connection->tx_seq << std::endl;
                                                    // std::cout << "D1 " << connection->tx_base << std::endl;
                if (connection->tx_seq == connection->tx_base ){
                    tx_ack = 1;   // all tx packets send and ack-ed
                    std::cout << "tx complete" << std::endl;
                    connection->tx_packets.clear();
                    connection->rx_packets.clear();
                    connection->tx_base = 0;
                    connection->tx_seq= 0;
                    stop_timer();

                    break;
                }
            }
        }
        // Project requirement
        sleep(1);
    }
    
    return NULL;
}


int handle_rx_packets() {
    
    struct connection_s *connection = connections;
    struct packet_s *packet;
    for ( ;connection != NULL; connection = connection->next) {
        /* loop through all connections */
        if (connection->type == CLIENT_CONNECTION  && connection->tx_packets.size() == 0)  {
            break;
        }
        if (connection->rx_packets.size() > connection->tx_base) {
                                                // std::cout << "D1" << connection->tx_base << std::endl;
            for ( int i = 0 ; i < connection->rx_packets.size(); i++) {      
                                                // std::cout << "D2" << std::endl;
                packet = connection->rx_packets.at(i);
                if (connection->tx_base == packet->seq-2) { // neg ack therefoe seq-1 ack and base need go up 1
                                                // std::cout << "D0 Move base to ..." << connection->tx_base+1 << std::endl;
                                                std::cout << "Stop timer form revc ..." << connection->tx_base+1 << std::endl;
                    stop_timer();              
                    packet = connection->tx_packets.at(connection->tx_base);
                    connection->tx_base++;
                    if (packet->type == PACKET_DATA) {
                        struct data_packet_s *data_packet = (struct data_packet_s *)packet;
                                                // std::cout << "D4 PACKT_BUFFER " << PACKET_BUFFER_SIZE << std::endl;
                                                // std::cout << "D5 data_packet.size " << data_packet->size << std::endl;
                        if ( data_packet->size != PACKET_BUFFER_SIZE ) {
                                                // std::cout << "End of file..." << std::endl;
                            return 1;

                            
                            // break;
                        }
                    }
                }
            }
        } 
     }
    return 0;
}

void *rx_exec(void *args) {
    if (connections == NULL) { 
        perror("No connection initialized.");
        exit(1);
    }
    
    /* timeout */
    
    fd_set readfds, excepfds, writefds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    
    FD_ZERO(&readfds);
    FD_ZERO(&excepfds);
    FD_ZERO(&writefds);
    
    /* timeout end */
    
    
    char buffer[PACKET_SIZE_ZCP];
    socklen_t addrlen = 0;
    while (1) {
        struct connection_s *connection = connections;
        for ( ;connection != NULL; connection = connection->next) {

            FD_SET(connection->fs, &readfds);
            FD_SET(connection->fs, &excepfds);
            FD_SET(connection->fs, &writefds);

            // memcpy(&readfds, &masterfds, sizeof(fd_set));
            
            if( select(connection->fs, &readfds, &writefds, &excepfds, &timeout) < 0){
                perror("select()");
                exit(1);
            }
            
            // if (FD_ISSET(connection->fs, &readfds)) {
           
                memset(buffer, 0, PACKET_SIZE_ZCP);
                                                    // std::cout << "D0 rx_exec" << std::endl;

                if ( recvfrom(connection->fs, 
                              buffer, 
                              PACKET_SIZE_ZCP, 0, 
                              (struct sockaddr*)&connection->addr,
                               &addrlen) < 0 ) {
                    perror("recvfrom()");
                    exit(4);
                 };
                                                   // std::cout << "D1 rx_exec" << std::endl;
                 receive_buffer(connection->type, connection->fs, connection->addr, buffer);
                                                   // std::cout << "D2 rx_exec" << std::endl;
                 if (handle_rx_packets() == 1) return NULL;
            // } else {
                                                               // std::cout << "D0 packet timeout" << std::endl;
            // }
            
            
                                                    // std::cout << "D1 packets " <<  connection->tx_packets.size()<< std::endl;
                                                    // std::cout << "D2 " <<  connection->type << std::endl;
             if (connection->type == CLIENT_CONNECTION  && connection->tx_packets.size() == 0)  {

                                                   // std::cout << "D3 rx_exec done" << std::endl;
                 return NULL;        
             }
        }
    }
                                                   // std::cout << "D4 rx_exec done" << std::endl;
}

void openZCP() {
    if (connections == NULL) { 
        perror("No connection initialized.");
        exit(1); 
    }
    pthread_create(&tx_t, 0, tx_exec, NULL);
    pthread_create(&rx_t, 0, rx_exec, NULL);
    pthread_join(tx_t,0);
    pthread_join(rx_t,0);
    // pthread_join(rx_t,0);
    // return 0;
  }