#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

using namespace std;

int main () {

        int sock, new_sock, nBytes, opt = 1;
        char buffer[1024] = {0};
        struct sockaddr_in serverAddr, clientAddr;
        int addrlen = sizeof(serverAddr);

        if (  sock == socket(AF_INET, SOCK_STREAM, 0 ) < 0 ) {
            cout << "ERROR\n";
            exit(EXIT_FAILURE);
        }

        
        cout << " Please enter the server IP: " ;
        cin.getline(buffer, 17, '\n');
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(buffer);
        cout << "Please enter port number: ";
        cin.getline(buffer,6, '\n'); 


        if ( connect(sock, (struct sockaddr*) &serverAddr, sizeof(serverAddr))< 0 ) {
               cout << "FAIL";
               exit(EXIT_FAILURE);
        }
       
        do 
        {
            cout << "Type Messsage: " << endl;
            cin.getline(buffer, 1024, '\n');
            if ( send(sock, buffer, strlen(buffer), 0) == -1) {
                cout << "Send failed" << endl;
                exit(EXIT_FAILURE);
            }
        } while ( strcmp(buffer, "Quit!"));

        cout << "Exit ...\n";
        return 0;


}