#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
// #include <ctype.h>
// #include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <time.h>

using namespace std;

int main()
{
    int MySocket, portNum, nBytes;
    char input_buffer[1024];
    char received_msg[1024];
    char port_string[10];
    char ip_string[17];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    int i = 0;
    std::string Query[] = {
        "car",
        "elephant",
        "flower",
        "monkey",
        "garbage"};

    /*Create UDP socket*/
    MySocket = socket(AF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    portNum = atoi("12345");
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
    srand(time(NULL));
    for (int count = 0; count < 10; count++)
    {
        i = rand() % 5;
        strcpy(input_buffer, Query[i].c_str());
        nBytes = strlen(input_buffer) + 1;
        sendto(MySocket, input_buffer, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
        cout << "Sent \e[31m" << input_buffer << "\e[0m to server...\n";
        nBytes = recvfrom(MySocket, received_msg, 1024, 0, NULL, NULL);
        cout << "Received from server: \e[34m" << received_msg << "\e[0m" << endl;
        sleep(1);
        memset(input_buffer, '\0', sizeof input_buffer);
    }
    strcpy(input_buffer, "Quit!");
    nBytes = strlen(input_buffer) + 1;
    sendto(MySocket, input_buffer, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
    return 0;
}