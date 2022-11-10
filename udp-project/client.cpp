#include <iostream>
#include <sys/socket.h> /* socket AF_INET SOCK_DGRAM  */
#include <netinet/in.h> /*  sockadrdr  */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>  /* clode */
int main () {
  
    char buffer[1024] = {0};
    int s;
    in_addr_t ip;
    int port;
    struct sockaddr_in server;

    std::cout << "Enter server ip address: 127.0.0.1" << std::endl;
    //cin.getline(buffer, 17, '\n'); 
    strcpy(buffer, "127.0.0.1");
    ip = inet_addr(buffer);
    
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
    server.sin_port = port;
    server.sin_addr.s_addr = ip;
    
    /* Send message */
    if( sendto(s, buffer,  strlen(buffer)+1, 0, 
                (const struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("sendto()");
        exit(3);
        
    }
    
    /*  Close socket*/
    close(s);
  
}