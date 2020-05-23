#ifndef __wlz_ConcurrentQRLock_HPP__
#define __wlz_ConcurrentQRLock_HPP__

# include <iostream>
# include <pthread.h>
# include <unistd.h>    // sleep
# include <stdint.h>    // uint32_t
# include <algorithm>

class ConcurrentQRLock{
public:
    explicit ConcurrentQRLock(){
        this->write_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->query_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->qw_cond    = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

        pthread_mutex_init(this->query_lock, NULL);
        pthread_mutex_init(this->write_lock, NULL);
        pthread_cond_init(this->qw_cond, NULL);

        this->query_count = 0;
        this->in_update = false;
    }
    virtual ~ConcurrentQRLock(){
        std::cout<<"delete ConcurrentQRLock"<<std::endl;
        /*没有意义
        while(this->query_count > 0 || this->in_update){
            sleep(1);
            std::cout<<"waiting for (this->query_count == 0 && !this->in_update())"<<std::endl;
        }
        */
        // 如果没有join等待线程执行完，则以下的释放会造成内存泄漏
        while(pthread_mutex_destroy(this->write_lock) != 0){
            std::cout<<"waiting for pthread_mutex_destroy(this->write_lock)"<<std::endl;
            sleep(1);
        }
        while(pthread_mutex_destroy(this->query_lock) != 0){
            std::cout<<"waiting for pthread_mutex_destroy(this->query_lock)"<<std::endl;
            sleep(1);
        }
        while(pthread_cond_destroy(this->qw_cond) != 0){
            std::cout<<"waiting for pthread_cond_destroy(this->qw_cond)"<<std::endl;
            sleep(1);
        }
        free(this->write_lock);
        free(this->query_lock);
        free(this->qw_cond);
    }
public:
    void before_collection_update(){
        pthread_mutex_lock(write_lock);
        in_update = true;
        pthread_mutex_lock(query_lock);
        while(query_count > 0){
            pthread_cond_wait(qw_cond, query_lock);
        }
    }
    void after_collection_update(){
        in_update = false;
        pthread_mutex_unlock(query_lock);
        pthread_mutex_unlock(write_lock);
        pthread_cond_broadcast(qw_cond);
    }
    void before_collection_query(){
        pthread_mutex_lock(query_lock);
        while(in_update){
            pthread_cond_wait(qw_cond, query_lock);
        }
        query_count += 1;
        pthread_mutex_unlock(query_lock);
    }
    void after_collection_query(){
        pthread_mutex_lock(query_lock);
        query_count -= 1;
        if(query_count == 0){
            pthread_mutex_unlock(query_lock);
            pthread_cond_broadcast(qw_cond);
        }else{
            pthread_mutex_unlock(query_lock);
        }
    }
    uint32_t get_query_count(){
        return query_count;
    }
    bool get_in_update(){
        return in_update;
    }
    void lock_query(){
        pthread_mutex_lock(query_lock);
        this->query_count += 1;
    }
    void unlock_query(){
        this->query_count -= 1;
        pthread_mutex_unlock(query_lock);
    }
    void lock_write(){
        pthread_mutex_lock(write_lock);
        this->in_update = true;
    }
    void unlock_write(){
        this->in_update = false;
        pthread_mutex_unlock(write_lock);
    }
private:
    pthread_mutex_t *write_lock;
    pthread_mutex_t *query_lock;
    pthread_cond_t  *qw_cond;
    bool in_update;
    int32_t query_count;
};
#endif
