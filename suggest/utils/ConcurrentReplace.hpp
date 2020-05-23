#include "ConcurrentTree.hpp"

class ConcurrentReplacer: public ConcurrentTree<bool, std::string>{
public:
    explicit ConcurrentReplacer(std::vector<uint8_t> *vec=NULL): ConcurrentTree(vec){
    }

    ~ConcurrentReplacer(){
        std::cout<<"delete ConcurrentReplacer"<<std::endl;
    }

    bool insert(const char *key, const char *val, bool force=false){
        return this->ConcurrentTree::insert(key, false, val, force);
    }

    std::string replace(const char * content){
        std::vector<PTInfo<std::string> > res_vec; res_vec.clear();
        this->ConcurrentTree::cut_max(content, res_vec);

        std::string res_str = "";
        for(uint32_t i = 0; i < res_vec.size(); i++){
            res_str += (res_vec[i].info == NULL? res_vec[i].word: (*res_vec[i].info));
        }
        return res_str;
    }

    void get_replace_info(const char * content,
                          std::vector<std::pair<std::string, std::string> >&res_vec){
        std::vector<PTInfo<std::string> > info_vec; info_vec.clear();
        this->ConcurrentTree::cut_max(content, info_vec);

        for(uint32_t i = 0; i < info_vec.size(); i++){
            if(info_vec[i].info == NULL){
                res_vec.push_back(std::make_pair(info_vec[i].word, ""));
            }else{
                res_vec.push_back(std::make_pair(info_vec[i].word, *info_vec[i].info));
            }
        }
    }
};
