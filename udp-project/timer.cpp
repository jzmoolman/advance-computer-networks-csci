#include <iostream>
#include <vector>
#include <unistd.h> /* sleep */
#include <pthread.h>
#include <random>
#include <time.h>

#include "timer.h"

pthread_t cid;
pthread_t pid;

pthread_mutex_t mutext1 = PTHREAD_MUTEX_INITIALIZER;

int produce = 0;

std::vector<int> list;

struct time_s {
  int timeout;
};

void* t_exec(void* args) {
    if (timer_started == 0 ) {
        timer_started = 1;
        struct time_s t = *(struct time_s *)args;
                                                        std::cout << "Timer up t=" << t.timeout << std::endl;
        while (1) {
            if (stop_timer_signal == 1) break;
            sleep(t.timeout );
            timeout_signal = 1;
                                                        std::cout << "Signal timeout" << std::endl;
        }
    }
                                                        std::cout << "Timer down" << std::endl;
    timer_started = 0;
    pthread_exit(NULL);
}

void* c_exec(void* args){
    while (1) {
        if (timeout_signal == 1) {
            timeout_signal = 0;
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

void start_timer(int time){
    if (timer_started) return;

    time_s *t = (struct time_s *)malloc(sizeof(struct time_s));
    t->timeout = time;
    pthread_create(&tid, 0, t_exec, t);
    pthread_detach(tid);
}

void stop_timer() {
    while (timer_started)
      stop_timer_signal = 1;

}
