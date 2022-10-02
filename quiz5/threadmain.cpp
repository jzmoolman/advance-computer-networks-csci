
#include <iostream>
#include <pthread.h>
#include <unistd.h>


using namespace std;

static int common_int = 0;
static int thrad_count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;



void* consume(void* buffer) {
    do {

        common_int--;
        cout << "consume: " << common_int << endl;
        // sleep(1);

        
    } while ( common_int > -600000);
    cout << "Consumer stopped" << endl;
    pthread_exit(NULL);
}

void* produce(void* buffer) {
    do {
        pthread_mutex_lock(&mutex1);
       common_int++;
       cout << "produce: " << common_int << endl;
        pthread_mutex_unlock(&mutex1);
       // sleep(1);
    } while (common_int < 60000);
    cout << "Producer stopped" << endl;
    pthread_exit(NULL);
}


int main() {

    pthread_t thread1;
    pthread_t thread2;
    int t1, t2;

    t1 = pthread_create(&thread1, NULL, consume,NULL);
    t2 = pthread_create(&thread2, NULL, produce,NULL);
    cout << "Thread 1 " << t1 <<  endl;

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;

}
