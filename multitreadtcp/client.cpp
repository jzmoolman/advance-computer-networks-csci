#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

using namespace std;

int main () {

        int sock = 0, nBytes, opt = 1;
        char buffer[1024] = {0};
        struct sockaddr_in serverAddr, clientAddr;
        int addrlen = sizeof(serverAddr);

        if ((  sock = socket(AF_INET, SOCK_STREAM, 0 )) == 0 ) {
            cout << "socket error" << endl;
            exit(EXIT_FAILURE);
        }

        
        cout << "Please enter the server IP: 127.0.0.1 ";
        strcpy(buffer, "127.0.0.1");
        //cin.getline(buffer, 17, '\n');
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(buffer);
        cout << "Please enter port number: 12345";
        strcpy(buffer, "12345");
        //cin.getline(buffer,6, '\n'); 
        serverAddr.sin_port = htons(atoi(buffer));

        if ( connect(sock, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0 ) {
               cout << "Connection fail";
               exit(EXIT_FAILURE);
        }
       
        do 
        {
            cout << "Type a messsage: "; 
            cin.getline(buffer, 1024, '\n');
            if ( send(sock, buffer, strlen(buffer) + 1, 0) == -1) 
            {
                cout << "Send failed" << endl;
                exit(EXIT_FAILURE);
                break;
            }
        } while ( strcmp(buffer, "Quit!") != 0);
        close(sock);
        cout << "Exit ...\n";
        return 0;
}
