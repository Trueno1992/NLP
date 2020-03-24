#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
using namespace std;
 
int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
 
void* produce(void* args)
{
    while(true)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        while(count >= 10)
        {
            sleep(1);
            cout << "product too much, sleep 1 second." << endl;
        }
        ++count;
        cout << "producer is  " << pthread_self() << "    count is  " << count << endl;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    pthread_exit(nullptr);
}
 
void* consume(void* args)
{
    while(true)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        while(count == 0)
        {
            cout << "product too little, sleep 1 second." << endl;
            pthread_cond_wait(&cond, &mutex);
        }
        --count;
        cout << "consumer is  " << pthread_self() << "    count is  " << count << endl;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(nullptr);
}
 
int main()
{
    pthread_t threadID_1 = 0;
    pthread_t threadID_2 = 0;
    
    const int ret1 = pthread_create(&threadID_1, 0, &produce, nullptr);
    const int ret2 = pthread_create(&threadID_2, 0, &consume, nullptr);
    
    if(ret1 == -1 || ret2 == -1)
    {
        return 0;
    }
    pthread_detach(threadID_1);
    pthread_detach(threadID_2);
    getchar();
    return 0;

}
