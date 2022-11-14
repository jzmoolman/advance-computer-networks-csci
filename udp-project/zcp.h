#ifndef __zcp__
#define __zcp__

#include <iostream> /* NULL */
#include <arpa/inet.h>  /* inet_addr */
#include <vector>

#include <pthread.h>

#define PACKET_SIZE_ZCP 1000 /* packet size */
#define WINDDOW_SIZE_ZCP 100 /* window size */

#define PACKET_ACK 1
#define PACKET_DATA 2

#define CLIENT_CONNECTION 1
#define SERVER_CONNECTION 2

struct packet_s {
  int type;
  unsigned int seq;
};

struct ack_packet_s {
  int type;
  unsigned int seq;
};

struct __data_packet_s {  /* for the time being just just for calc the size of data buffer */
  int type;
  unsigned int seq;
  unsigned int size;
};

const int  PACKET_BUFFER_SIZE = PACKET_SIZE_ZCP - sizeof(struct __data_packet_s);

struct data_packet_s {
  int type;
  unsigned int seq;
  unsigned int size;
  // char buffer[PACKET_SIZE_ZCP-sizeof(__data_packet_s)];
  char buffer[PACKET_BUFFER_SIZE];
};

struct connection_s {
    int type;
    int fs;
    struct sockaddr_in addr;
    struct connection_s *next;
    int tx_base;
    int tx_seq;
    pthread_mutex_t tx_mutex;
    pthread_mutex_t rx_mutex;
  
    std::vector<struct packet_s * > tx_packets;
    std::vector<struct packet_s * > rx_packets;
};

void print_connections();
void print_connection(struct connection_s *connection, int summary);
void print_packet(packet_s *packet);
void print_addr(struct sockaddr_in addr);
struct connection_s *lookup_connection(int type, int fd, struct sockaddr_in addr); 
void receive_buffer(int type, int fs, struct sockaddr_in addr, char *buffer);
void loadFile(char* filename, std::vector<struct packet_s* > *list);
void openZCP();

#endif
