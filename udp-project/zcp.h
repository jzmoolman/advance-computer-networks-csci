#ifndef __zcp__
#define __zcp__

#include <iostream> /* NULL */
#include <arpa/inet.h>  /* inet_addr */
#include <vector>

#define PACKET_SIZE_ZCP 1000 /* packet size */
#define WINDDOW_SIZE_ZCP 100 /* window size */

struct packet_s {
  int type;
};


#define PACKET_ACK 1
#define PACKET_DATA 2

struct data_packet_s {
  int type;
  unsigned int seq;
  unsigned int size;
  char buffer[PACKET_SIZE_ZCP-2*sizeof(unsigned int )];
};

struct ack_packet_s {
  int type;
  unsigned int seq;
};

struct connection_s {
    struct sockaddr_in addr;
    struct connection_s *next;
    int base;
    std::vector<struct packet_s * > packets;
};

void print_connections();
void print_connection(struct connection_s *connection, int summary);
void print_packet(packet_s *packet);
void print_addr(struct sockaddr_in addr);
struct connection_s *lookup_connection(struct sockaddr_in addr); 
void receive_buffer(struct sockaddr_in addr, char *buffer);
void loadFile(char* filename, std::vector<struct packet_s* > *list);

#endif
