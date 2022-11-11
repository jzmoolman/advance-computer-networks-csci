#ifndef __zcp__
#define __zcp__

#include <iostream> /* NULL */
#include <arpa/inet.h>  /* inet_addr */
#include <vector>

#define PACKET_SIZE_ZCP 1000 /* packet size */
#define WINDDOW_SIZE_ZCP 100 /* window size */


struct packet_s {
  unsigned int seq;
  unsigned int size;
  char buffer[PACKET_SIZE_ZCP-2*sizeof(unsigned int )];
};

struct client_s {
    struct sockaddr_in addr;
    struct client_s *next;
    int base;
    std::vector<struct packet_s * > *packets;
};

void print_clients();
void print_addr(struct sockaddr_in addr);
struct client_s *lookup_client(struct sockaddr_in addr); 
void new_client_buffer(struct sockaddr_in addr, char* buffer);
void new_packet_list(std::vector<struct packet_s * > * list, char* buffer, int size);
void loadFile(char* filename, std::vector<struct packet_s* > *list);
#endif
