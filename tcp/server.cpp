#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
//#include <arpa/inet.h> 
#include <unistd.h>

using namespace std;

int main () {

        int sock, new_sock, nBytes, opt = 1;
        char buffer[1024] = {0};
        struct sockaddr_in serverAddr, clientAddr;
        int addrlen = sizeof(serverAddr);

        if (  sock == socket(AF_INET, SOCK_STREAM, 0 ) < 0 ) {
            cout << "socket failed" << endl;
            exit(EXIT_FAILURE);

        } else {
            cout << "socket: sock " << sock << endl;
        }

        int errorCode =  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); 
        if ( errorCode == -1  ) {
            cout << "setsockopt error: errorCode " << errno << endl;
            exit(EXIT_FAILURE);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        cout << "Please enter port number: ";

        cin.getline(buffer,6, '\n'); 


        if ( ::bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0 ) {
            cout << "Bind Failed!" << endl;
            exit(EXIT_FAILURE);

        };

        cout << "Listening ..." << endl;
        if ( listen(sock, 3)  < 0 )
        {
            cout << "Listen Failed!" << endl;
            exit(EXIT_FAILURE);
        }

        if (( new_sock = accept(sock, (struct sockaddr *)&serverAddr, (socklen_t *)&addrlen  ))  < 0) {
            cout << "Accept Failed!!" << endl;
            exit(EXIT_FAILURE);
        }

        do {
            nBytes = read(new_sock, buffer, 1024);
            buffer[nBytes];
            cout << "Server Receieved " << buffer << endl;
            send(new_sock, buffer, strlen(buffer), 0);
            if ( strcmp(buffer, "Quit!") == 0 ) {
                close(new_sock);
            }
            memset(buffer, '\0', sizeof(buffer));
        } while ( strcmp(buffer, "Quit!"));

        cout << "Exit ...\n";
        close(sock);
        return 0;


}
