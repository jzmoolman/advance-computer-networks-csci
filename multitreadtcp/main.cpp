#include <iostream>
#include <pthread.h>


using namespace std;

void* printMessage(void *ptr)
{
    char* message;
    message = (char*)ptr;
    cout << message << endl;
}

int main() {

    pthread_t thread1;
    pthread_t thread2;
    const char* message1 = "thread1";
    const char* message2 = "thread2";
    int t1, t2;

    t1 = pthread_create(&thread1, NULL, printMessage, (void*)message1);
    t2 = pthread_create(&thread2, NULL, printMessage, (void*)message2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;

}
