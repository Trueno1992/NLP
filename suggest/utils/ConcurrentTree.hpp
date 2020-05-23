#ifndef __wlz_ConcurrentTree__HPP__
#define __wlz_ConcurrentTree__HPP__

#include <iostream>
#include <stdint.h>
#include <string.h>
#include <Tree.hpp>
#include <QWLock.hpp>

template<class W>
bool is_greater(const W &a, const W &b){
    if(a > b) return true;
    return false;
}

template<class W, class T, uint64_t top_num=UINT64_MAX, bool (*gt)(const W &, const W &)=is_greater>
class ConcurrentTree: private Tree<W, T, top_num, gt>, public ConcurrentQRLock{
public:
    explicit ConcurrentTree(std::vector<uint8_t> *vec=NULL): Tree<W, T, top_num, gt>(vec), ConcurrentQRLock(){
    }
    virtual ~ConcurrentTree(){
        std::cout<<"delete concurrentTree"<<std::endl;
    }
    bool insert(const char * content, const W &w, const T &info, bool force=false){
        before_collection_update();
        try{
            bool res = this->Tree<W, T, top_num, gt>::insert(content, w, info, force);
            after_collection_update();
            return res;
        }catch(...){
            after_collection_update();
            throw;
        }
    }
    bool remove(const char * content){
        before_collection_update();
        try{
            bool res = this->Tree<W, T, top_num, gt>::remove(content);
            after_collection_update();
            return res;
        }catch(...){
            after_collection_update();
            throw;
        }
    }
    T * get_info(const char * content){
        before_collection_query();
        try{
            T * info = this->Tree<W, T, top_num, gt>::get_info(content);
            after_collection_query();
            return info;
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    uint32_t get_suffix_count(const char * content){
        before_collection_query();
        try{
            uint32_t count = this->Tree<W, T, top_num, gt>::get_suffix_count(content);
            after_collection_query();
            return count;
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    void get_suffix_info(const char * content,
                         std::vector<PTWInfo<W, T> > &vec,
                         const uint32_t c_limit){
        before_collection_query();
        try{
            this->Tree<W, T, top_num, gt>::get_suffix_info(content, vec, c_limit);
            after_collection_query();
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    void get_lcp_suffix_info(const char * content,
                         std::vector<PTWInfo<W, T> > &vec,
                         const uint32_t c_limit){
        before_collection_query();
        try{
            this->Tree<W, T, top_num, gt>::get_lcp_suffix_info(content, vec, c_limit);
            after_collection_query();
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    void get_prefix_info(const char * content,
			             std::vector<PTInfo<T> > &vec,
						 const uint32_t c_limit){
        before_collection_query();
        try{
            this->Tree<W, T, top_num, gt>::get_prefix_info(content, vec);
            after_collection_query();
        }catch(...){
            after_collection_query();
            throw;
        }
    }
    void cut_max(const char * content, std::vector<PTInfo<T> > &vec){
        before_collection_query();
        try{
            this->Tree<W, T, top_num, gt>::cut_max(content, vec);
            after_collection_query();
        }catch(...){
            after_collection_query();
            throw("cut_max error");
        }
    }
    void lock_query(){
        this->ConcurrentQRLock::lock_query();
    }
    void unlock_query(){
        this->ConcurrentQRLock::unlock_query();
    }
};
#endif
