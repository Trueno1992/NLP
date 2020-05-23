#ifndef __wlz_Suggest_hpp_
#define __wlz_Suggest_hpp_

#include <stdint.h>
#include <vector>
#include <iostream>

template<class W, class T, uint64_t top_num, bool (*gt)(const W &, const W &)>
class Suggest: public Tree<W, T, top_num, gt>{
public:
    explicit Suggest(std::vector<uint8_t> *vec=NULL): Tree<W, T, top_num, gt>(vec){
    }
    ~Suggest(){
        std::cout<<"delete Suggest"<<std::endl;
    }
};
#endif
