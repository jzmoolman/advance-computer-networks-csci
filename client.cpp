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
        char* clientIP;

        udpSocket = socket(AF_INET,  SOCK_DGRAM,0);
        serverAddr.sin_family = AF_INET;

        cout << "Please enter port number: 12345" << endl;
        // cin.getline(buffer, 9,'\n');
        strcpy(buffer, "12345\n");
        serverAddr.sin_port = htons(atoi(buffer));

        cout << "Please enter Server IP: 127.0.0.1" << endl;
        // cin.getline(buffer, 16, '\n');
        strcpy(buffer, "127.0.0.1\n");
        serverAddr.sin_addr.s_addr = inet_addr(buffer);

        memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

        bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        // bind(udpSocket, &serverAddr, sizeof(serverAddr));


       socklen_t addr_size = sizeof(serverAddr);
        do {
            //sending a command
            cout << "Type someting: ";
            cin.getline(buffer, 1024, '\n');
            nBytes  = strlen(buffer) + 1;
            sendto(udpSocket, buffer, nBytes, 0, (struct sockaddr*)&serverAddr, addr_size);

            nBytes  = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr*)&serverAddr, &addr_size);
            buffer[nBytes] = '\0';
            clientIP = inet_ntoa(clientAddr.sin_addr);
            cout << clientIP << " says: " << buffer << endl;



        } while (strcmp(buffer, "quit") != 0);
        close(udpSocket);
        return 0;
}