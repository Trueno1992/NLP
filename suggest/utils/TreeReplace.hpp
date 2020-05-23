#ifndef __wlz_Replacer_hpp__
#define __wlz_Replacer_hpp__

#include "stdint"

class Replacer: public Tree<bool, std::string>{
public:
    explicit Replacer(std::vector<uint8_t> *vec=NULL): Tree(vec){
    }

    ~Replacer(){
        std::cout<<"delete Replacer"<<std::endl;
    }

    bool insert(const char *key, const char *val, bool force=false){
        return this->Tree::insert(key, false, val, force);
    }

    std::string replace(const char * content){
        std::vector<PTInfo<std::string> > res_vec; res_vec.clear();
        this->Tree::cut_max(content, res_vec);

        std::string res_str = "";
        for(uint32_t i = 0; i < res_vec.size(); i++){
            res_str += (res_vec[i].info == NULL? res_vec[i].word: (*res_vec[i].info));
        }
        return res_str;
    }
};
#endif
