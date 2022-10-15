#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <sys/ioctl.h>

using namespace std;

void* respond(void *arg);
int thread_count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_args  {
  int sock;
  struct sockaddr_in address;
} thread_args_t;

int main(int argc, char**argv) {

  int serverSocket = 0, new_socket, port, opt = 1;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  pthread_t tid[100];
  
  // Creating socket file descriptor
  if (( serverSocket = socket(AF_INET, SOCK_STREAM, 0 )) == 0 ) {

    cout << "socket error" << errno << endl;
    exit(EXIT_FAILURE);
  } 

  // Force serverSocket scoket to reuse ip and port
//  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt) ) == -1) {
  if (setsockopt(serverSocket, SOL_SOCKET,  SO_REUSEPORT, &opt, sizeof(opt) ) == -1) {
    cout << "setsocketop error:" << errno << endl;
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  cout << "Please enter listning port: 12345" << endl; 
  strcpy(buffer, "12345");
  //cin.getline(buffer, 7, \n);
  address.sin_port = htons(atoi(buffer));  
  if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0 ) {
    cout << "bind error" << endl;
    exit(EXIT_FAILURE);
  }
  cout << "Listen to new tcp request..." << endl;
  if ( listen( serverSocket, 10) < 0 ) {
    cout << "listen error" << endl;
    exit(EXIT_FAILURE);
  }
  thread_args_t *args; 
  while (1) {

    if (( new_socket = ::accept(serverSocket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0 ) {
      cout << "accept failed" << endl;
      exit(EXIT_FAILURE);
    } else {
      cout << "New connection!" << endl;
      args = (thread_args_t*)malloc(sizeof(thread_args_t));
      args->sock = new_socket;
      args->address = address;
      pthread_create(&tid[thread_count], NULL, respond, args);
      pthread_detach(tid[thread_count]);
      pthread_mutex_lock(&mutex1);
      thread_count++;
      pthread_mutex_unlock(&mutex1);
      cout << "# connections: " << thread_count << endl;
    }
    while (thread_count > 99 ) {
      sleep(1);
    }
  }
  return 0;
}

int parseMessage(char *buffer ) {

  char _buffer[1024] = {0};
  for ( int i = 0; i < 1024; i++ ) {
    _buffer[i] = buffer[i];
    if ( strcmp(_buffer, "GET") == 0 ) {
      cout << _buffer << endl;
      return 1;  // GET Command
    }
  } 
  return -1;
}

void* respond(void* buffer) {
    char message[1024] = {0};
    char clientip[20];
    int nBytes;
    char *temp, *t;
    bool done = false;
    int size;
    char c[2];

    int command = 0;

    bool failure = false;
    //new_socket = *((int*)buffer);
    thread_args_t args = *((thread_args_t*)buffer);
    struct sockaddr_in addr;
    socklen_t addres_size = sizeof(struct sockaddr_in);
    //in nat enc you can only get router ip
    //strcpy(clientip, inet_ntoa(addr.sin_addr));
    strcpy(clientip, inet_ntoa(args.address.sin_addr));
    cout << "Client IP: " << clientip << endl;
    do {
      memset(message, '\0', sizeof(message));
      size = 0;
      // read a line at a time, message end with 0
      do {
        nBytes = read(args.sock, c, 1);
        if (nBytes == 0) {

          if (c[0] != 0 ) {
            failure = true;
            break; // something bad had happend
          }
        }
        message[size] = c[0];
        
        if ( message[size-1] == '\r' && message[size] == '\n' ) {
          if (size != 1) {
            cout << message;
            switch( command ) {
              case 0: {
                  command = parseMessage(message);
                  memset(message, '\0', sizeof(message));
                  size = 0;
              }
              case 1 : {  //GET COMMAND
                  //eat up rest of command
                  break;
              }
              default: {
                  failure = true;
              }
            }

            size = 0;
          } else { // Return Return => end of message
            c[0] = '\0';
          }
        } else { 
          size++;
        }
      } while ((c[0] != 0) && (!failure)); // end when reaches 0
                                           //
      cout << "after while !=0" << endl;
      if  (!failure) {
        cout << "Command " << command  << endl;
        switch (command) {
            case 1: {
                int error_code;
                unsigned int error_code_size = sizeof(error_code);
                if (getsockopt(args.sock, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size)==0) {
                    cout << "getsockopt == 0" << endl;
                    if (error_code == 0) {
                      cout << "Received from << " << clientip << ": " << message << endl;
                      if (strcmp(message, "Quit!") != 0) {
                        send(args.sock, message, strlen(message)+ 1, 0); // send message
                                                                          // back to user
                      }
                    } else {
                      cout << "getsockopt != 0" << endl;
                      failure = true;
                      break;
                    }
                          }
            }
            default: {
                break;
            }
        }
          
        } else {
          failure = true;
          break;
        }

      }  else {
        cout << "Unknow command/eorro reading socket"  << endl;
        break;
      } 
    } while ( strcmp(message, "Quit!") != 0 );
    cout << "\033[1;31mClient disconnected!\033[0m" << endl;
    pthread_mutex_lock(&mutex1);
    thread_count--;
    cout << "\033[1;31mNumber of  connections: " << thread_count << "\033[0m" << endl;
    pthread_mutex_unlock(&mutex1);
    close(args.sock);
    pthread_exit(NULL);
}




