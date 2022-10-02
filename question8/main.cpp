#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

using namespace std;

int main () {

        int udpSocket, nBytes;
        char buffer[1024];
        struct sockaddr_in serverAddr, clientAddr;
        char * clientIP;
        udpSocket = socket(PF_INET,  SOCK_DGRAM,0);
        serverAddr.sin_family = AF_INET;

        cout << "Please enter port number: 12345\n";

        // cin >> buffer;
        strcpy(buffer, "12345\n");

        serverAddr.sin_port = htons(atoi(buffer));
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

        bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        // bind(udpSocket, &serverAddr, sizeof(serverAddr));


       socklen_t addr_size = sizeof(clientAddr); do {
            // recieve message
            nBytes  = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr*)&clientAddr, &addr_size);
            buffer[nBytes] = '\0';
            clientIP = inet_ntoa(clientAddr.sin_addr);
            cout << clientIP << " client: " << buffer << endl;

            // send reply back
            if ( strcmp(buffer, "car") == 0) {
                strcpy(buffer, "fast");
                nBytes = strlen(buffer) + 1;
            } else if ( strcmp(buffer, "flower") == 0) {
                strcpy(buffer, "beautiful");
                nBytes = strlen(buffer) + 1;
            } else if ( strcmp(buffer, "elephant") == 0) {
                strcpy(buffer, "mouse");
                nBytes = strlen(buffer) + 1;
            } else if ( strcmp(buffer, "monkey") == 0) {
                strcpy(buffer, "banana");
                nBytes = strlen(buffer) + 1;
            } else if (strcmp(buffer, "Quit!") == 0) {
              break;
            } else {
                strcpy(buffer, "what?");
                nBytes = strlen(buffer) + 1;
            }
            sendto(udpSocket, buffer, nBytes, 0, (struct sockaddr*)&clientAddr, addr_size);
        } while (strcmp(buffer, "quit") != 0);
        close(udpSocket);
        return 0;
}
