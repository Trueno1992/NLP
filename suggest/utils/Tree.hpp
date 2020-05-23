#ifndef __wlz_TreeBase_HPP__
#define __wlz_TreeBase_HPP__

# include <map>
# include <vector>
# include <stdio.h>
# include <iostream>
# include <string.h>
# include <stdint.h>    // uint32_t
# include <utility>     // 包含pair
# include <queue>
# include <StrUtils.h>  // UBase::U32ToUtf8
# include <algorithm>

namespace UBase{

namespace Struct{

template<class Weight>
bool is_greater(const Weight &a, const Weight &b){
    if(a > b) return true;
    return false;
}

template<class Weight>
struct Type{
    typedef bool (*CmpFunc)(const Weight &, const Weight &);
};

template<class Info> struct PcInfo;
template<class Weight, class Info> struct PwInfo;

template<class Weight, class Info> class Son;
template<class Weight, class Info> class Node;
template<class Weight, class Info> class NodeArr;
template<class Weight, class Info> class NodeMap;
template<class Weight, class Info, uint64_t top_num=UINT64_MAX, typename Type<Weight>::CmpFunc gt=is_greater> class Tree;
template<class Weight, class Info> NodeMap<Weight, Info> * arr2map(NodeArr<Weight, Info> *arrNode);


template<class Info> struct PcInfo{
    std::string word;
    uint32_t offset;
    uint32_t len;
    const Info * info;
    PcInfo(){
        len    = 0;
        word   = "";
        info   = NULL;
        offset = 0;
    }
};

template<class Weight, class Info> struct PwInfo{
    std::string word;
    uint32_t len;
    const Weight * weight;
    const Info * info;
    PwInfo(){
        len    = 0;
        word   = "";
        info   = NULL;
        weight = NULL;
    }
};

template<class Weight, class Info> class Node{
public:
    Node(){
        this->in_num = 0;
        this->is_end = false;
        this->weight = NULL;
        this->info   = NULL;
    }
    virtual ~Node(){
        //std::cout<<"delete node"<<std::endl;
        delete weight;
        this->weight = NULL;
        delete info;
        this->info   = NULL;
    };

    virtual void clear_son_top_wei(){};
    virtual void dset_son(const uint32_t &word, Son<Weight, Info> *)=0;
    virtual Son<Weight, Info> * get_son(const uint32_t &word)=0;
    virtual Son<Weight, Info> * set_son(const uint32_t &word, const Weight &w, const uint8_t &arrLen)=0;
    virtual Son<Weight, Info> * gset_son(const uint32_t &word, const Weight &w, std::vector<uint8_t> &arrLen_vec)=0;
    virtual Weight * get_node_max_wei(typename Type<Weight>::CmpFunc temp_gt)=0;
    virtual void set_wei(const Weight &tmp_wei){
        if(this->weight != NULL) delete this->weight;
        this->weight = new Weight(tmp_wei);
    }
    virtual void set_info(const Info &tmp_info){
        if(this->info != NULL) delete this->info;
        this->info = new Info(tmp_info);
    }
public:
    int32_t in_num;
    bool is_end;
    Weight * weight;
    Info * info;
};

template<class Weight, class Info> class Son{
public:
    explicit Son(){
        this->node = NULL;
        this->max_wei = NULL;
        this->is_arr = false;
    }
    void to_arrSon(const Weight &wei, const uint8_t arrLen){
        this->is_arr = true;
        this->set_max_wei(wei);
        this->node = new NodeArr<Weight, Info>(arrLen);
    }
    void to_mapSon(const Weight &wei){
        this->is_arr = false;
        this->set_max_wei(wei);
        this->node = new NodeMap<Weight, Info>();
    }
    void set_max_wei(const Weight &wei){
        if(this->max_wei != NULL)
            delete this->max_wei;
        this->max_wei = new Weight(wei);
    }
    bool update_max_wei(const Weight &wei, bool (*tmp_gt)(const Weight &, const Weight &)){
        if(this->max_wei != NULL && tmp_gt(wei, *this->max_wei)){
            delete this->max_wei;
            this->max_wei = new Weight(wei);
            return true;
        }
        return false;
    }
    ~Son(){
        this->release();
    }
    void release(){
        delete this->node;
        this->node = NULL;
        delete this->max_wei;
        this->max_wei = NULL;
    }
public:
    Weight * max_wei;
    Node<Weight, Info> *node;
    bool is_arr;
};

template<class Weight, class Info> class NodeMap: public Node<Weight, Info>{
public:
    explicit NodeMap(): Node<Weight, Info>(){
        this->son_next_map = NULL;
        this->son_top_vec = NULL;
    }
    explicit NodeMap(NodeArr<Weight, Info> *nodeArr): Node<Weight, Info>(){
         this->weight = nodeArr->weight;
         this->is_end = nodeArr->is_end;
         this->in_num = nodeArr->in_num;
         this->info   = nodeArr->info;

         this->son_next_map = new std::map<uint32_t, Son<Weight, Info> * >;
         this->son_top_vec = new std::vector<std::pair<uint32_t, Weight *> >;
         for(int32_t i = 0; i < nodeArr->son_num; i++){
             if(nodeArr->all_son[i] == NULL) continue;
             (*this->son_next_map)[nodeArr->all_word[i]] = nodeArr->all_son[i];
         }
    }
    ~NodeMap(){
        //std::cout<<"delete mapnode"<<std::endl; 
        if(this->son_next_map != NULL){
            typename std::map<uint32_t, Son<Weight, Info> * >::iterator it;
            for(it=this->son_next_map->begin(); it != this->son_next_map->end(); ++it){
                delete it->second;
                it->second = NULL;
            }
            this->son_next_map->clear();
            delete this->son_next_map;
            this->son_next_map = NULL;
        }
        if(this->son_top_vec != NULL){
            this->son_top_vec->clear();
            delete this->son_top_vec;
            this->son_top_vec=NULL;
        }
    }
    void clear_son_top_wei(){
        this->son_top_vec->clear();
    }
    void dset_son(const uint32_t &word, Son<Weight, Info> *);
    Son<Weight, Info> * get_son(const uint32_t &word);
    Son<Weight, Info> * set_son(const uint32_t &word, const Weight &w, const uint8_t &arrLen);
    Son<Weight, Info> * gset_son(const uint32_t &word, const Weight &w, std::vector<uint8_t> &arrLen_vec);
    Weight * get_node_max_wei(typename Type<Weight>::CmpFunc temp_gt);
public:
    std::map<uint32_t, Son<Weight, Info> * > *son_next_map;
    std::vector<std::pair<uint32_t, Weight *> > *son_top_vec;
};

template<class Weight, class Info> class NodeArr: public Node<Weight, Info>{
public:
    explicit NodeArr(uint8_t son_num): Node<Weight, Info>(){
        this->son_num = son_num;
        this->all_son = new Son<Weight, Info>*[son_num];
        this->all_word = new uint32_t[son_num];
        for(int32_t i = 0; i < this->son_num; i++){
            this->all_son[i] = NULL;
            this->all_word[i] = 0;
        }
    }
    ~NodeArr(){
        //std::cout<<"delete arrnode"<<std::endl; 
        if(this->all_son != NULL){
            for(int32_t i = 0; i < this->son_num; i++){
                if(this->all_son[i] == NULL) continue;
                delete this->all_son[i];
                this->all_son[i] = NULL;
            }
            delete this->all_son;
            this->all_son = NULL;
        }
        if(this->all_word != NULL){
            delete [] (this->all_word);
            this->all_word = NULL;
        }
        this->son_num = 0;
    }
    void resize(const uint8_t &new_son_num);
    bool resize(std::vector<uint8_t> &arrLen_vec);
    void clear_son_top_wei(){}
    void dset_son(const uint32_t &word, Son<Weight, Info> * son);
    Son<Weight, Info> * get_son(const uint32_t &word);
    Son<Weight, Info> * set_son(const uint32_t &word, const Weight &w, const uint8_t &arrLen);
    Son<Weight, Info> * gset_son(const uint32_t &word, const Weight &w, std::vector<uint8_t>&arrLen_vec);
    Weight * get_node_max_wei(typename Type<Weight>::CmpFunc temp_gt);
public:
    Son<Weight, Info> **all_son;
    uint32_t *all_word;
    uint8_t son_num;
};

template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt> class Tree{
public:
    explicit Tree(std::vector<uint8_t> *arrLen_init_vec=NULL){
        this->arrLen_vec.clear();
        if(arrLen_init_vec != NULL && arrLen_init_vec->size() != 0){
            if(arrLen_init_vec->size() > 50)throw("vector.size() <= 50 limit?");
            uint8_t pre = 0;
            for(uint32_t i = 0; i < arrLen_init_vec->size(); i++){
                if((*arrLen_init_vec)[i] <= pre) throw("input value must increase");
                if((*arrLen_init_vec)[i] >= UINT8_MAX)throw("input value must uint8_t");
                this->arrLen_vec.push_back((*arrLen_init_vec)[i]);
                pre = (*arrLen_init_vec)[i];
            }
        }else{
            this->arrLen_vec.push_back(1);
            this->arrLen_vec.push_back(3);
            this->arrLen_vec.push_back(6);
        }
        this->root = new Son<Weight, Info>();
    }
    virtual ~Tree(){
        delete this->root;
        this->root = NULL;

        this->arrLen_vec.clear();
        std::cout<<"delete tree"<<std::endl;
    }

public:
    bool insert(const char *, const Weight &, const Info &, bool);
    bool remove(const char *);
    Info * get_info(const char *);
    uint32_t get_suffix_count(const char *);
    void cut_max(const char *, std::vector<PcInfo<Info> > &);
    void get_prefix_info(const char *, std::vector<PcInfo<Info> > &);
    void get_suffix_info(const char *, std::vector<PwInfo<Weight, Info> > &, const uint32_t);
    std::string get_lcp_suffix_info(const char *, std::vector<PwInfo<Weight, Info> > &, const uint32_t);
    virtual void lock_query(){};
    virtual void unlock_query(){};
    //friend NodeMap<Weight, Info> * arr2map(NodeArr<Weight, Info> *);

//private:
public:
    bool gen_max_pcinfo(std::vector<uint32_t>::iterator begin,
                        std::vector<uint32_t>::iterator end,
                        PcInfo<Info> &pcinfo);

    Son<Weight, Info> * get_endp(const char *content);

    Son<Weight, Info> * get_endp(std::vector<uint32_t>::iterator begin,
                                 std::vector<uint32_t>::iterator end);

    Son<Weight, Info> * get_lcp_endp(const char *content,
                                     std::string &pre_str);

    Son<Weight, Info> * get_lcp_endp(std::vector<uint32_t>::iterator begin,
                                     std::vector<uint32_t>::iterator end,
                                     std::string &pre_str);

    void get_suffix_info(Son<Weight, Info> *, const char *, std::vector<PwInfo<Weight, Info> > &, const uint32_t);
//private:
public:
    Son<Weight, Info> *root;
    std::vector<uint8_t> arrLen_vec;
};

template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
bool Tree<Weight, Info, top_num, gt>::insert(const char *content,
                                             const Weight &weight,
                                             const Info &info,
                                             bool force=false){
    if(this->root->node == NULL) this->root->to_arrSon(weight, this->arrLen_vec[0]);

    Son<Weight, Info> * son = this->root; Son<Weight, Info> * next = NULL;

    std::vector<uint32_t> vec_words; vec_words.clear();
    if(!UBase::Utils::Utf8ToU32(content, vec_words)) return false;

    std::vector<Son<Weight, Info> *> son_path_vec; son_path_vec.clear();
    for(std::vector<uint32_t>::iterator word_it = vec_words.begin(); word_it != vec_words.end(); word_it++){ 
        son_path_vec.push_back(son);
        next = son->node->gset_son(*word_it, weight, this->arrLen_vec);
        if(next == NULL){
            son->node = arr2map((NodeArr<Weight, Info> *)son->node);
            son->is_arr = false;
            next = son->node->gset_son(*word_it, weight, this->arrLen_vec);
        }
        son = next;
    }
    if(son->node->is_end && !force)return false;
    bool gt_son_max = (!son->node->is_end || weight >= (*son->node->weight));
    son->node->set_wei(weight);
    son->node->set_info(info);
    son->node->clear_son_top_wei();
    if(gt_son_max){
        for(uint32_t i = 0; i < son_path_vec.size(); i++){
            son_path_vec[i]->update_max_wei(weight, gt);
            son_path_vec[i]->node->in_num += (son->node->is_end? 0: 1);
            son_path_vec[i]->node->clear_son_top_wei();
        }
        son->node->in_num += (son->node->is_end? 0: 1); 
    }else{
        son->max_wei = son->node->get_node_max_wei(gt);
        for(uint32_t i = son_path_vec.size() - 1; i >= 0; --i){
            son_path_vec[i]->set_max_wei(*son_path_vec[i]->node->get_node_max_wei(gt));
            son_path_vec[i]->node->in_num += 0;
            son_path_vec[i]->node->clear_son_top_wei();
        }
        son->node->in_num += 0; 
    }
    son->node->is_end = true;
    return true;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
bool Tree<Weight, Info, top_num, gt>::remove(const char *content){
    if(this->root->node == NULL) return true;
    Son<Weight, Info> * son = this->root; Son<Weight, Info> * next = NULL;

    std::vector<uint32_t> vec_words;
    if(!UBase::Utils::Utf8ToU32(content, vec_words)) return false;

    std::vector<std::pair<Son<Weight, Info> *, uint32_t> > son_path_vec; son_path_vec.clear();
    son_path_vec.push_back(std::make_pair(son, 0));
    for(std::vector<uint32_t>:: iterator word_it = vec_words.begin(); word_it != vec_words.end(); word_it++){ 
        next = son->node->get_son(*word_it);
        if(next == NULL)return false;
        son = next;
        son_path_vec.push_back(std::make_pair(son, *word_it));
    }
    if(!son->node->is_end)return false;

    son->node->is_end = false;
    for(int32_t i = son_path_vec.size() - 1; i >= 0; --i){
        son_path_vec[i].first->node->in_num -= 1;
        if(son_path_vec[i].first->node->in_num <= 0){
            if(i == 0){
                son_path_vec[i].first->release();
            }else{
                son_path_vec[i-1].first->node->dset_son(son_path_vec[i].second, NULL);
            }
        }else{
            son_path_vec[i].first->set_max_wei(*son_path_vec[i].first->node->get_node_max_wei(gt));
            son_path_vec[i].first->node->clear_son_top_wei();
        }
    }
    return true;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
Son<Weight, Info> * Tree<Weight, Info, top_num, gt>::get_endp(std::vector<uint32_t>:: iterator start,
                                                              std::vector<uint32_t>:: iterator end){
    Son<Weight, Info> *son = this->root;
    for(std::vector<uint32_t>:: iterator it = start; it != end; it++){
        Son<Weight, Info> * next_son = son->node->get_son(*it);
        if(next_son == NULL) return NULL;
        son = next_son;
    }
    return son;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
Son<Weight, Info> * Tree<Weight, Info, top_num, gt>::get_endp(const char *content){
    std::vector<uint32_t> candidate_words;
    if(!UBase::Utils::Utf8ToU32(content, candidate_words)) return NULL;
    return this->get_endp(candidate_words.begin(), candidate_words.end());
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
Son<Weight, Info> * Tree<Weight, Info, top_num, gt>::get_lcp_endp(std::vector<uint32_t>:: iterator start,
                                                                  std::vector<uint32_t>:: iterator end,
                                                                  std::string &pre_str){
    Son<Weight, Info> *son = this->root;
    for(std::vector<uint32_t>:: iterator it = start; it != end; it++){
        Son<Weight, Info> * next_son = son->node->get_son(*it);
        if(next_son == NULL) break;
        UBase::Utils::U32ToUtf8(*it, pre_str, true);
        son = next_son;
    }
    return son;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
Son<Weight, Info> * Tree<Weight, Info, top_num, gt>::get_lcp_endp(const char *content,
                                                                  std::string &pre_str){
    std::vector<uint32_t> candidate_words;
    if(!UBase::Utils::Utf8ToU32(content, candidate_words)) return NULL;
    return this->get_lcp_endp(candidate_words.begin(), candidate_words.end(), pre_str);
}
template<class Weight, typename Type<Weight>::CmpFunc gt>
struct cmp1 {//从小到大
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        return (gt(*left.first.first->max_wei, *right.first.first->max_wei));
    }
};
template<class Weight, typename Type<Weight>::CmpFunc gt>
struct cmp2 {//从小到大
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        //std::cout<<*left.first.first->node->weight<<std::endl;
        //std::cout<<*right.first.first->node->weight<<std::endl;
        //std::cout<<!gt(*left.first.first->node->weight, *right.first.first->node->weight)<<std::endl;
        return (gt(*left.first.first->node->weight, *right.first.first->node->weight));
    }
};
template<class wtype, class Weight, typename Type<Weight>::CmpFunc gt>
bool cmp4(const std::pair<wtype, Weight *> &a, const std::pair<wtype, Weight *> &b) {
    return gt(*a.second, *b.second);
    //return *a.second > *b.second; // 大的在前
};

template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
void Tree<Weight, Info, top_num, gt>::get_suffix_info(Son<Weight, Info> *son,
                                                      const char * prefix_str,
                                                      std::vector<PwInfo<Weight, Info> > &res_list,
                                                      const uint32_t c_limit){
    typedef std::pair<std::pair<Son<Weight, Info> *, std::string>, int> Qnode_info;
    std::priority_queue<Qnode_info, std::vector<Qnode_info>, cmp1<Weight, gt> >queue;
    while(!queue.empty()) queue.pop();
    std::priority_queue<Qnode_info, std::vector<Qnode_info>, cmp2<Weight, gt> >result;
    while(!queue.empty()) queue.pop();

    typename std::map<uint32_t, Son<Weight, Info> * >::iterator son_it;
    typename std::vector<std::pair<uint32_t, Weight *> >::iterator top_it;
    typename std::vector<Qnode_info>::iterator node_it;

    std::vector<Qnode_info> all_nodes;
    queue.push(std::make_pair(std::make_pair(son, std::string(prefix_str)), 0));
    while(queue.size()){
        all_nodes.clear();
        while(queue.size()){
            all_nodes.push_back(queue.top()); queue.pop();
        }
        reverse(all_nodes.begin(), all_nodes.end());
        for(node_it = all_nodes.begin(); node_it != all_nodes.end(); node_it++){
            Son<Weight, Info> * son = node_it->first.first;
            
            Weight * son_max_wei = son->max_wei;

            if(result.size() >= c_limit && gt(*result.top().first.first->node->weight,   *son_max_wei)) break;
            if(result.size() >= c_limit &&    *result.top().first.first->node->weight == *son_max_wei)  break;

            if(son->node->is_end){
                result.push(*node_it);
                while(result.size() > c_limit) result.pop();
            }

            if(!son->is_arr){
                NodeMap<Weight, Info> * nodeMap = (NodeMap<Weight, Info> *)son->node;
                this->lock_query();
                if(nodeMap->son_next_map->size() != 0 && nodeMap->son_top_vec->size() == 0){
                    for(son_it = nodeMap->son_next_map->begin(); son_it != nodeMap->son_next_map->end(); son_it++){
                        if(son_it->second == NULL)continue;
                        nodeMap->son_top_vec->push_back(std::make_pair(son_it->first, son_it->second->max_wei));
                    }
                    sort(nodeMap->son_top_vec->begin(), nodeMap->son_top_vec->end(), cmp4<uint32_t, Weight, gt>);
                }
                this->unlock_query();
                int j = 0;
                for(top_it = nodeMap->son_top_vec->begin(); top_it != nodeMap->son_top_vec->end(); top_it++){
                    if(queue.size() >= c_limit && gt(*queue.top().first.first->max_wei,   *top_it->second))  break;
                    if(queue.size() >= c_limit &&    *queue.top().first.first->max_wei == *top_it->second)   break;
                    if(result.size() >= c_limit && gt(*result.top().first.first->node->weight,   *top_it->second))  break;
                    if(result.size() >= c_limit &&    *result.top().first.first->node->weight == *top_it->second)   break;
                    std::string tmp_str = node_it->first.second; UBase::Utils::U32ToUtf8(top_it->first, tmp_str, true);
                    queue.push(std::make_pair(std::make_pair((*nodeMap->son_next_map)[top_it->first], tmp_str), node_it->second + 1));
                    while(queue.size() > c_limit) queue.pop();
                    if((++j) > c_limit)throw("get_suffix_info error sort");
                }
            }else{
                NodeArr<Weight, Info> * nodeArr = (NodeArr<Weight, Info> *)son->node;
                for(int32_t i = 0; i < nodeArr->son_num; i++){
                    if(nodeArr->all_son[i] == NULL)continue;
                    if(queue.size() >= c_limit && gt(*queue.top().first.first->max_wei,   *nodeArr->all_son[i]->max_wei)) continue;
                    if(queue.size() >= c_limit &&    *queue.top().first.first->max_wei == *nodeArr->all_son[i]->max_wei)  continue;
                    if(result.size() >= c_limit && gt(*result.top().first.first->node->weight,   *nodeArr->all_son[i]->max_wei)) continue;
                    if(result.size() >= c_limit &&    *result.top().first.first->node->weight == *nodeArr->all_son[i]->max_wei)  continue;
                    std::string tmp_str = node_it->first.second; UBase::Utils::U32ToUtf8(nodeArr->all_word[i], tmp_str, true);
                    queue.push(std::make_pair(std::make_pair(nodeArr->all_son[i], tmp_str), node_it->second + 1));
                    while(queue.size() > c_limit) queue.pop();
                }
            }
            while(queue.size() > c_limit) queue.pop();
            while(result.size() > c_limit) result.pop();
        }
    }
    uint32_t prefix_len = UBase::Utils::getU32len(prefix_str);
    while(result.size() > c_limit) result.pop();
    while(result.size() != 0){
        Qnode_info tmp = result.top();
        //std::cout<<*tmp.first.first->node->weight<<std::endl;
        PwInfo<Weight, Info> rinfo;
        rinfo.word   = tmp.first.second;
        rinfo.len    = prefix_len + tmp.second;
        rinfo.weight = tmp.first.first->node->weight;
        rinfo.info   = tmp.first.first->node->info;
        res_list.push_back(rinfo);
        result.pop();
    }
    reverse(res_list.begin(), res_list.end());
}

template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
void Tree<Weight, Info, top_num, gt>::get_suffix_info(const char * content,
                                                      std::vector<PwInfo<Weight, Info> > &res_list,
                                                      const uint32_t c_limit){
    if(this->root->node == NULL) return;
    Son<Weight, Info> *son = this->get_endp(content);
    if(son == NULL) return;
    this->get_suffix_info(son, content, res_list, c_limit);
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
std::string Tree<Weight, Info, top_num, gt>::get_lcp_suffix_info(const char * content,
                                                                 std::vector<PwInfo<Weight, Info> > &res_list,
                                                                 const uint32_t c_limit){
    if(this->root->node == NULL) return "";
    std::string pre_str = "";
    Son<Weight, Info> *son = this->get_lcp_endp(content, pre_str);
    if(son == NULL) return "";
    this->get_suffix_info(son, pre_str.c_str(), res_list, c_limit);
    return pre_str;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
uint32_t Tree<Weight, Info, top_num, gt>::get_suffix_count(const char *content){
    if(this->root->node == NULL) return 0;
    Son<Weight, Info> *son = this->get_endp(content);
    if(son == NULL) return 0;
    return son->node->in_num;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
Info * Tree<Weight, Info, top_num, gt>::get_info(const char *content){
    if(this->root->node == NULL) return NULL;
    Son<Weight, Info> *son = this->get_endp(content);
    if(son == NULL) return NULL;
    return son->node->info;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
void Tree<Weight, Info, top_num, gt>::get_prefix_info(const char * content,
                                                      std::vector<PcInfo<Info> > &res_list){
    if(this->root->node == NULL) return;
    std::vector<uint32_t> words_vec;
    if(!UBase::Utils::Utf8ToU32(content, words_vec)) return;

    Son<Weight, Info> *son = this->root; std::string tmp_word = ""; uint32_t offset = 0;
    for(std::vector<uint32_t>:: iterator it = words_vec.begin(); it != words_vec.end(); it++){
        if(son->node->is_end){
            PcInfo<Info> rInfo;
            rInfo.word   = tmp_word;
            rInfo.len    = offset + 1;
            rInfo.offset = offset;
            rInfo.info   = son->node->info;
            res_list.push_back(rInfo);
        }
        Son<Weight, Info> * next_son = son->node->get_son(*it);
        if(next_son == NULL) return;
        son = next_son;
        offset += 1;
        tmp_word += *it;
    }
}

template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
bool Tree<Weight, Info, top_num, gt>::gen_max_pcinfo(std::vector<uint32_t>::iterator begin,
                                                     std::vector<uint32_t>::iterator end,
                                                     PcInfo<Info> &pcinfo){
    if(begin == end) return false;

    UBase::Utils::U32ToUtf8(*begin, pcinfo.word);
    pcinfo.info   = NULL;
    pcinfo.len    = 1;

    if(this->root->node == NULL) return false;
    Son<Weight, Info> * son = this->root; int32_t len = 0;
    for(std::vector<uint32_t>::iterator it = begin; it != end; it++){
        Son<Weight, Info> * next = son->node->get_son(*it);
        if(next == NULL) break;
        len += 1; son = next;
        if(son->node->is_end){
            UBase::Utils::U32ToUtf8(begin, it + 1, pcinfo.word);
            pcinfo.info   = son->node->info;
            pcinfo.len    = len;
        }
    }
    return true;
}
template<class Weight, class Info, uint64_t top_num, typename Type<Weight>::CmpFunc gt>
void Tree<Weight, Info, top_num, gt>::cut_max(const char *content, std::vector<PcInfo<Info> > &res){

    std::vector<uint32_t> vec_words;
    if(!UBase::Utils::Utf8ToU32(content, vec_words)) return;

    int32_t position = 0; 
    for(std::vector<uint32_t>::iterator it = vec_words.begin(); it != vec_words.end();){
        PcInfo<Info> pcinfo;
        pcinfo.offset = position;
        //std::cout<<"22"<<std::endl;
        this->gen_max_pcinfo(it, vec_words.end(), pcinfo);
        //std::cout<<"33"<<std::endl;
        res.push_back(pcinfo);
        it       += pcinfo.len;
        position += pcinfo.len;
    }
}
template<class Weight, class Info>
NodeMap<Weight, Info> * arr2map(NodeArr<Weight, Info> *arrNode){
    NodeMap<Weight, Info> *nodemap = new NodeMap<Weight, Info>(arrNode);
    for(int32_t i = 0; i < arrNode->son_num; i++){
        arrNode->all_son[i]  = NULL;
        arrNode->all_word[i] = 0;
    }
    arrNode->weight = NULL;
    arrNode->info   = NULL;
    delete arrNode;
    return nodemap;
}


template<class Weight, class Info>
void NodeArr<Weight, Info>::resize(const uint8_t &new_son_num){
    Son<Weight, Info> **new_all_son = new Son<Weight, Info>*[new_son_num];
    uint32_t *new_all_word = new uint32_t[new_son_num];

    for(int32_t i = 0; i < new_son_num; ++i){
        new_all_son[i]  = NULL;
        new_all_word[i] = 0;
    }

    for(int32_t i = 0; i < this->son_num; ++i){
        new_all_son[i]    = this->all_son[i];
        this->all_son[i]  = NULL;
        new_all_word[i]   = this->all_word[i];
        this->all_word[i] = 0;
    }

    delete all_son;
    delete [] all_word;

    this->son_num  = new_son_num;
    this->all_son  = new_all_son;
    this->all_word = new_all_word;
}
template<class Weight, class Info>
bool NodeArr<Weight, Info>::resize(std::vector<uint8_t> &arrLen_vec){
    for(int32_t i = 0; i < arrLen_vec.size(); i++){
        if(arrLen_vec[i] > this->son_num){
            this->resize(arrLen_vec[i]);
            return true;
        }
    }
    return false;
}
template<class Weight, class Info>
Weight * NodeArr<Weight, Info>::get_node_max_wei(typename Type<Weight>::CmpFunc temp_gt){
    if(this->in_num <= 0) throw("NodeArr<Weight, Info>::get_node_max_wei this->in_num <=0 error");
    Weight * w = NULL;

    if(this->is_end) w = this->weight;

    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] != NULL){
            if(w == NULL || temp_gt(*this->all_son[i]->max_wei, *w)){
                w = this->all_son[i]->max_wei;
            }
        }
    }
    if(w == NULL) throw("NodeArr<Weight, Info>::get_node_max_wei w == NULL error");
    return w;
}
template<class Weight, class Info>
void NodeArr<Weight, Info>::dset_son(const uint32_t &word, Son<Weight, Info> *son){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL || this->all_word == NULL)continue;
        if(this->all_word[i] == word){
            delete this->all_son[i];
            this->all_son[i] = son;
            return;
        }
    }
    throw("set_son cant`t find this son");
}
template<class Weight, class Info>
Son<Weight, Info> * NodeArr<Weight, Info>::get_son(const uint32_t &word){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL)continue;
        if(this->all_word[i] == word) return all_son[i];
    }
    return NULL;
}
template<class Weight, class Info>
Son<Weight, Info> * NodeArr<Weight, Info>::set_son(const uint32_t &word,
                                                   const Weight &wei,
                                                   const uint8_t &arrLen){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL){
            all_son[i] = new Son<Weight, Info>();
            all_son[i]->to_arrSon(wei, arrLen);
            all_word[i] = word;
            return all_son[i];
        }
    }
    return NULL;
}
template<class Weight, class Info>
Son<Weight, Info> * NodeArr<Weight, Info>::gset_son(const uint32_t &word,
                                                    const Weight &wei,
                                                    std::vector<uint8_t> &arrLen_vec){
    Son<Weight, Info> * son = this->get_son(word);
    if(son != NULL) return son;

    son = this->set_son(word, wei, arrLen_vec[0]);
    if(son != NULL) return son;

    if(this->resize(arrLen_vec)){
        return this->gset_son(word, wei, arrLen_vec);
    }
    return NULL;
}


template<class Weight, class Info>
void NodeMap<Weight, Info>::dset_son(const uint32_t &word,
                                     Son<Weight, Info> *son){
    delete (*this->son_next_map)[word];
    (*this->son_next_map)[word] = son;
}
template<class Weight, class Info>
Son<Weight, Info> * NodeMap<Weight, Info>::get_son(const uint32_t &word){
    typename std::map<uint32_t, Son<Weight, Info> * >::iterator it = this->son_next_map->find(word);
    if(it != this->son_next_map->end()) return it->second;
    return NULL;
}
template<class Weight, class Info>
Son<Weight, Info> * NodeMap<Weight, Info>::set_son(const uint32_t &word,
                                                   const Weight &wei,
                                                   const uint8_t &arrLen){
    Son<Weight, Info> * son = new Son<Weight, Info>();
    son->to_arrSon(wei, arrLen);
    (*this->son_next_map)[word] = son;
    return son;
}
template<class Weight, class Info>
Son<Weight, Info> * NodeMap<Weight, Info>::gset_son(const uint32_t &word,
                                                    const Weight &wei,
                                                    std::vector<uint8_t> &arrLen_vec){
    Son<Weight, Info> *son = this->get_son(word);
    if(son != NULL) return son;
    return this->set_son(word, wei, arrLen_vec[0]);
}
template<class Weight, class Info>
Weight * NodeMap<Weight, Info>::get_node_max_wei(typename Type<Weight>::CmpFunc temp_gt){
    if(this->in_num <= 0) throw("NodeMap<Weight, Info>::get_node_max_wei in_num <= 0 error");
    Weight * wei = NULL;
    if(this->is_end) wei = this->weight;

    typename std::map<uint32_t, Son<Weight, Info> * >::iterator son_it;
    for(son_it = this->son_next_map->begin(); son_it != this->son_next_map->end(); ++son_it){
        if(son_it->second == NULL) continue;
        if(wei == NULL || temp_gt(*son_it->second->max_wei,  *wei)){
            wei = son_it->second->max_wei;
        }
    }
    if(wei == NULL) throw("NodeMap<Weight, Info>::get_node_max_wei wei == NULL error");
    return wei;
}

}  // end namespace Struct

}  // end namespace UBase
#endif
