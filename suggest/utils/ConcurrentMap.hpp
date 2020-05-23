#include <iostream>
#include <map>
#include <QWLock.hpp>

template<class K, class V>
class ConcurrentMap: public ConcurrentQRLock{
public:
    explicit ConcurrentMap(): ConcurrentQRLock(){
    }
    ~ConcurrentMap(){
        std::cout<<"delete concurrentmap"<<std::endl;
    }
    void clear(){
        before_collection_update();
        try{
            this->tmap.clear();
            after_collection_update();
        }catch(...){
            after_collection_update();
            throw;
        }
    }
    void insert(const K &key, const V &val){
        before_collection_update();
        try{
            this->tmap[key] = val;
            after_collection_update();
        }catch(...){
            after_collection_update();
            throw;
        }
    }
    bool erase(const K &key){
        before_collection_update();
        try{
            bool is_rease = false;
            typename std::map<K, V>::iterator it = this->tmap.find(key);
            if(it !=this->tmap.end()){
                this->tmap.erase(key);
                is_rease = true;
            }
            after_collection_update();
            return is_rease;
        }catch(...){
            after_collection_update();
            throw;
        }
    }
    uint32_t size(){
        before_collection_query();
        try{
            uint32_t _size = this->tmap.size();
            after_collection_query();
            return _size;
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    bool find(const K &key, V &val){
        before_collection_query();
        try{
            bool is_find = false;
            typename std::map<K, V>::iterator it = this->tmap.find(key);
            if(it != this->tmap.end()){
                val = it->second;
                is_find = true;
            }
            after_collection_query();
            return is_find;
        }catch(...){
            after_collection_query();
            throw;
        }
    }
	void empty(){
        before_collection_query();
        try{
			this->map->empty();
        }catch(...){
            after_collection_query();
            throw;
        }
	}
public:
    std::map<K, V> tmap;
};
