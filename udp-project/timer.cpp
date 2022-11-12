#include <iostream>
#include <vector>
#include <unistd.h> /* sleep */
#include <pthread.h>
#include <random>
#include <time.h>

pthread_t cid;
pthread_t pid;
pthread_t tid;

pthread_mutex_t mutext1 = PTHREAD_MUTEX_INITIALIZER;

int produce = 0;
int timeout = 0;

std::vector<int> list;

struct time_s {
  int timeout;
};

void* t_exec(void* args){
    while (1) {
        struct time_s t = *(struct time_s *)args;
        sleep(t.timeout);
        timeout = 1;
        // std::cout << "Time out" << std::endl;
    }
    return NULL; 
}

void* c_exec(void* args){
    while (1) {
        if (timeout == 1) {
            timeout = 0;
            std::cout << "Consumer: Time out sig" << std::endl;
            if (list.size() > 0 ){
                pthread_mutex_lock(&mutext1);
                int number = list.at(list.size()-1);
                std::cout << "Consumer: " << number << std::endl;
                list.pop_back();
                pthread_mutex_unlock(&mutext1);
            }
            produce = 1;
        }
    }
    return NULL; 
}

void* p_exec(void* args){
  
   srand(time(NULL));
  
    while (1) {
        if (produce == 1) {
            produce = 0;
            std::cout << "Producer: produce sig" << std::endl;
            pthread_mutex_lock(&mutext1);
            list.push_back(rand() % 100);
            pthread_mutex_unlock(&mutext1);
        }
        sleep(1);
    }
  
    return NULL; 
}


int main () {
    std::cout << "Timer up" << std::endl;

    time_s t;
    t.timeout = 2;
    pthread_create(&tid, 0, t_exec, &t);
    pthread_create(&pid, 0, p_exec, NULL);
    pthread_create(&cid, 0, c_exec, NULL);
  
    sleep(10);
    pthread_kill(pid, 0);
    pthread_kill(cid, 0);

    std::cout << "Timer down" << std::endl;
  
  return 0;

}
