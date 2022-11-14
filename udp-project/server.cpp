#include <iostream>
#include <sys/socket.h> /* socket AF_INET SOCK_DGRAM  */
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>  /* close */

#include "zcp.h"

int main () {

    char buffer[PACKET_SIZE_ZCP] = {0};

    int s;
    in_addr_t ip;
    int port;
    struct sockaddr_in server, client;
    socklen_t serverlen, clientlen;

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
    
    serverlen= sizeof(server);
    if (getsockname(s, (struct sockaddr*)&server, &serverlen) < 0) {
        perror("bind()");
        exit(3);
    }
    std::cout << "Server ip: " << inet_ntoa(server.sin_addr) << std::endl;
    std::cout << "Server port: " << ntohs(server.sin_port) << std::endl;
  
    clientlen = sizeof(client);
    
    while (1) {
        if ( recvfrom(s, buffer, PACKET_SIZE_ZCP, 0, (struct sockaddr*)&client, &clientlen) < 0 ) {
            perror("recvfrom()");
            exit(4);
         };

        receive_buffer(SERVER_CONNECTION,s, client, buffer);
        
        // send ack
        int seq = 0;
        struct packet_s *rx_packet =  (struct packet_s *)buffer;
        if ( rx_packet->type == PACKET_DATA) {
            seq = ((struct data_packet_s *)buffer)->seq; 
            struct packet_s tx_packet;
            tx_packet.type = PACKET_ACK;
            tx_packet.seq = rx_packet->seq +1;
            print_packet(&tx_packet);

            if ( sendto(s, &tx_packet, sizeof(struct ack_packet_s), 0, 
                             (const struct sockaddr *)&client, clientlen) <  0 ) {
                perror("sendto()");
                exit(999);
            }
        }
    }
  
    /*  Close socket*/
    close(s);
  
}
