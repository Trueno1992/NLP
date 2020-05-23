#ifndef __wlz_ConcurrentSuggest_HPP__
#define __wlz_ConcurrentSuggest_HPP__

template<class W, class T, uint64_t top_num, bool (*gt)(const W &, const W &)>
class ConcurrentSuggest: public ConcurrentTree<W, T, top_num, gt>{
public:
    explicit ConcurrentSuggest(std::vector<uint8_t> *vec=NULL): ConcurrentTree<W, T, top_num, gt>(vec){
    }
    ~ConcurrentSuggest(){
        std::cout<<"delete concurrentSuggest"<<std::endl;
    }
};

#endif
