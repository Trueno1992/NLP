#ifndef __lzhouwu_suggest_H_
#define __lzhouwu_suggest_H_

# include <map>
# include <vector>
# include <stdio.h>
# include <iostream>
# include <pthread.h>
# include <string.h>
# include <stdint.h> // uint32_t
# include <utility>  // 包含pair
# include <queue>
# include <StrUtils.h>
# include <algorithm>


template<class W, class T> struct ResInfo;
template<class W, class T> class Node;
template<class W, class T> class Son;
template<class W, class T> class NodeArr;
template<class W, class T> class NodeMap;

template<class W> W mmax(W a, W b){
    if(a >= b) return a;
    return b;
}

template<class W, class T> struct ResInfo{
    std::string phrase;
    uint32_t position;
    uint32_t phrase_len;
    W weight;
    T info;
};

template<class W, class T> class Node{
public:
    virtual void clear_son_top_wei(){};
    virtual void set_son(uint32_t, Son<W, T> *)=0;
    virtual Son<W, T> * get_son(uint32_t)=0;
    virtual Son<W, T> * set_son(uint32_t, W, uint8_t)=0;
    virtual Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t>&)=0;
    virtual W get_node_max_wei()=0;
    virtual ~Node(){};
public:
    int32_t in_num;
    bool is_end;
    W weight;
    T info;
};

template<class W, class T> class Son{
public:
    Son(){
        this->node = NULL;
        this->is_arr = false;
    }
    void to_arrSon(W wei, uint8_t arrLen){
        this->is_arr = true;
        this->max_wei = wei;
        this->node = new NodeArr<W, T>(arrLen);
    }
    void to_mapSon(W wei){
        this->is_arr = false;
        this->max_wei = wei;
        this->node = new NodeMap<W, T>();
    }
    ~Son(){
        if(this->node == NULL) return;
        /*
        if(this->is_arr){
            delete (NodeArr<W, T> *)this->node;
        }else{
            delete (NodeMap<W, T> *)this->node;
        }
        */
        delete this->node;
        this->node = NULL;
    }
    void release_son(){
        if(this->node == NULL) return;
        /*if(this->is_arr){
            delete (NodeArr<W, T> *)this->node;
        }else{
            delete (NodeMap<W, T> *)this->node;
        }
        */
        delete this->node;
        this->node = NULL;
    }
public:
    W max_wei;
    Node<W, T> *node;
    bool is_arr;
};

template<class W, class T> class NodeMap: public Node<W, T>{
public:
    NodeMap(){
        this->son_next_map = NULL;
        this->son_top_vec = NULL;
    }
    NodeMap(NodeArr<W, T> *nodeArr){
         this->weight = nodeArr->weight;
         this->is_end = nodeArr->is_end;
         this->in_num = nodeArr->in_num;
         this->son_next_map = new std::map<uint32_t, Son<W, T> * >;
         this->son_top_vec = new std::vector<std::pair<uint32_t, W> >;

         for(int32_t i = 0; i < nodeArr->son_num; i++){
             if(nodeArr->all_son[i] == NULL) continue;
             (*this->son_next_map)[nodeArr->all_word[i]] = nodeArr->all_son[i];
         }
    }
    ~NodeMap(){
        //std::cout<<"delete mapnode"<<std::endl; 
        if(this->son_next_map != NULL){
            typename std::map<uint32_t, Son<W, T> * >::iterator it;
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

    void set_son(uint32_t, Son<W, T> *);

    Son<W, T> * get_son(uint32_t);

    Son<W, T> * set_son(uint32_t, W, uint8_t);

    Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t> &);

    W get_node_max_wei();

public:
    std::map<uint32_t, Son<W, T> * > *son_next_map;
    std::vector<std::pair<uint32_t, W> > *son_top_vec;
};

template<class W, class T> class NodeArr: public Node<W, T>{
public:
    NodeArr(uint8_t son_num){
        this->is_end = false;
        this->in_num = 0;
        this->son_num = son_num;
        this->all_son = new Son<W, T>*[son_num];
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
    void resize(uint8_t);

    bool resize(std::vector<uint8_t> &);

    void clear_son_top_wei(){}

    void set_son(uint32_t, Son<W, T> *);

    Son<W, T> * get_son(uint32_t);

    Son<W, T> * set_son(uint32_t, W, uint8_t);

    Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t>&);

    W get_node_max_wei();
public:
    Son<W, T> **all_son;
    uint32_t *all_word;
    uint8_t son_num;
};

template<class W, class T, uint32_t top_num> class Tree{
public:
    Tree(std::vector<uint8_t> *arrLen_init_vec){
        this->arrLen_vec.clear();
        if(arrLen_init_vec != NULL && arrLen_init_vec->size() != 0){
            if(arrLen_init_vec->size() > 50)throw("vector.size() <= 50 limit?");
            uint32_t pre = 0;
            for(uint32_t i = 0; i < arrLen_init_vec->size(); i++){
                if((*arrLen_init_vec)[i] <= pre) throw("input value must increase");
                this->arrLen_vec.push_back((*arrLen_init_vec)[i]);
                pre = (*arrLen_init_vec)[i];
            }
        }else{
            this->arrLen_vec.push_back(1);
            this->arrLen_vec.push_back(3);
            this->arrLen_vec.push_back(6);
        }
        this->root = new Son<W, T>();
    }
    ~Tree(){
        delete this->root;
        this->root = NULL;
        this->arrLen_vec.clear();
        //std::cout<<"delete tree"<<std::endl;
    }

    bool insert(const char *, W, T, bool);
    bool remove(const char *);
    T * get_info(const char *);
    uint32_t get_suffix_count(const char *);
    void get_suffix_info(const char *, std::vector<ResInfo<W, T> > &, const uint32_t);

    Son<W, T> * get_endp(const char *content);
    Son<W, T> * get_endp(std::vector<uint32_t>::iterator, std::vector<uint32_t>::iterator);
    void get_suffix_info(Son<W, T> *, const char *, std::vector<ResInfo<W, T> > &, const uint32_t);
    NodeMap<W, T> * arr2map(NodeArr<W, T> *);
    Son<W, T> *root;
private:

public:
    std::vector<uint8_t> arrLen_vec;
};

template<class W, class T, uint32_t top_num> class ConcurrentTree: public Tree<W, T, top_num>{
public:
    ConcurrentTree(std::vector<uint8_t> *arrLen_init_vec): Tree<W, T, top_num>(arrLen_init_vec){
        this->write_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->query_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->qw_cond = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

        pthread_mutex_init(this->write_lock, NULL);
        pthread_cond_init(this->qw_cond, NULL);

        this->query_count = 0;
        this->in_update = false;
    }
    ~ConcurrentTree(){
        free(this->write_lock);
        free(this->query_lock);
        free(this->qw_cond);
        //std::cout<<"delete concurrentTree"<<std::endl;
    }
    pthread_mutex_t *write_lock;
    pthread_mutex_t *query_lock;
    pthread_cond_t  *qw_cond;
    bool in_update;
    int32_t query_count;
};

template<class W, class T, uint32_t top_num>
bool Tree<W, T, top_num>::insert(const char *content, W weight, T info, bool force=false){
    if(this->root->node == NULL){
        this->root->to_arrSon(weight, this->arrLen_vec[0]);
    }

    Son<W, T> * son = this->root; Son<W, T> * next_son = NULL;

    std::vector<uint32_t> vec_words; vec_words.clear();
    if(!Utf8ToU32(content, vec_words)) return false;

    std::vector<Son<W, T> *> son_path_vec; son_path_vec.clear();
    for(std::vector<uint32_t>::iterator word_it = vec_words.begin(); word_it != vec_words.end(); word_it++){ 
        son_path_vec.push_back(son);
        next_son = son->node->gset_son(*word_it, weight, this->arrLen_vec);
        if(next_son == NULL){
            son->node = this->arr2map((NodeArr<W, T> *)son->node);
            son->is_arr = false;
            next_son = son->node->gset_son(*word_it, weight, this->arrLen_vec);
        }
        son = next_son;
    }
    if(son->node->is_end && !force)return false;
    son->node->weight = weight;
    son->node->info = info;
    son->node->clear_son_top_wei();
    if(!son->node->is_end || weight >= son->node->weight){
        for(uint32_t i = 0; i < son_path_vec.size(); i++){
            son_path_vec[i]->max_wei = mmax(son_path_vec[i]->max_wei, weight);
            son_path_vec[i]->node->in_num += (son->node->is_end? 0: 1);
            son_path_vec[i]->node->clear_son_top_wei();
        }
        son->node->in_num += (son->node->is_end? 0: 1); 
    }else{
        son->max_wei = son->node->get_node_max_wei();
        for(uint32_t i = son_path_vec.size() - 1; i >= 0; --i){
            son_path_vec[i]->max_wei = son_path_vec[i]->node->get_node_max_wei();
            son_path_vec[i]->node->in_num += 0;
            son_path_vec[i]->node->clear_son_top_wei();
        }
        son->node->in_num += 0; 
    }
    son->node->is_end = true;
}
template<class W, class T, uint32_t top_num>
bool Tree<W, T, top_num>::remove(const char *content){
    if(this->root->node == NULL) return true;
    Son<W, T> * son = this->root; Son<W, T> *next_son = NULL;

    std::vector<uint32_t> vec_words;
    if(!Utf8ToU32(content, vec_words)) return false;

    std::vector<std::pair<Son<W, T> *, uint32_t> > son_path_vec; son_path_vec.clear();
    son_path_vec.push_back(std::make_pair(son, 0));
    for(std::vector<uint32_t>:: iterator word_it = vec_words.begin(); word_it != vec_words.end(); word_it++){ 
        next_son = son->node->get_son(*word_it);
        if(next_son == NULL)return false;
        son = next_son;
        son_path_vec.push_back(std::make_pair(son, *word_it));
    }
    if(!son->node->is_end)return false;

    son->node->is_end = false;
    for(int32_t i = son_path_vec.size() - 1; i >= 0; --i){
        son_path_vec[i].first->node->in_num -= 1;
        if(son_path_vec[i].first->node->in_num <= 0){
            if(i == 0){
                son_path_vec[i].first->release_son();
            }else{
                delete son_path_vec[i].first;
                son_path_vec[i-1].first->node->set_son(son_path_vec[i].second, NULL);
            }
        }else{
            son_path_vec[i].first->max_wei = son_path_vec[i].first->node->get_node_max_wei();
            son_path_vec[i].first->node->clear_son_top_wei();
        }
    }
    return true;
}
template<class W, class T, uint32_t top_num>
Son<W, T> * Tree<W, T, top_num>::get_endp(std::vector<uint32_t>:: iterator start,
                                          std::vector<uint32_t>:: iterator end){
    Son<W, T> *son = this->root;
    for(std::vector<uint32_t>:: iterator it = start; it != end; it++){
        Son<W, T> * next_son = son->node->get_son(*it);
        if(next_son == NULL) return NULL;
        son = next_son;
    }
    return son;
}
template<class W, class T, uint32_t top_num>
Son<W, T> * Tree<W, T, top_num>::get_endp(const char *content){
    std::vector<uint32_t> candidate_words;
    if(!Utf8ToU32(content, candidate_words)) return NULL;
    return this->get_endp(candidate_words.begin(), candidate_words.end());
}
struct cmp1 {//从小到大
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        if (left.first.first->max_wei > right.first.first->max_wei) return true;
        return false;
    }
};
struct cmp2 {//从小到大
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        if (left.first.first->node->weight > right.first.first->node->weight) return true;
        return false;
    }
};
template<class wtype, class W>
bool cmp3(const std::pair<wtype, W> a, const std::pair<wtype, W> b) {
    return a.second > b.second; // 大的在前
};
template<class W, class T, uint32_t top_num>
void Tree<W, T, top_num>::get_suffix_info(Son<W, T> *son,
                                          const char * prefix_str,
                                          std::vector<ResInfo<W, T> > &res_list,
                                          const uint32_t c_limit){
    std::priority_queue<std::pair<std::pair<Son<W, T> *, std::string>, int>, std::vector<std::pair<std::pair<Son<W, T> *, std::string>, int> >, cmp1>queue;
    while(!queue.empty()) queue.pop();
    std::priority_queue<std::pair<std::pair<Son<W, T> *, std::string>, int>, std::vector<std::pair<std::pair<Son<W, T> *, std::string>, int> >, cmp2>result;
    while(!queue.empty()) queue.pop();

    queue.push(std::make_pair(std::make_pair(son, std::string(prefix_str)), 0));
    std::vector<std::pair<std::pair<Son<W, T> *, std::string>, int> > all_nodes;
    typename std::map<uint32_t, Son<W, T> * >::iterator son_it;
    typename std::vector<std::pair<uint32_t, W> >::iterator top_it;
    typename std::vector<std::pair<std::pair<Son<W, T> *, std::string>, int> >::iterator node_it;
    while(queue.size()){
        all_nodes.clear();
        while(queue.size()){
            all_nodes.push_back(queue.top()); queue.pop();
        }
        reverse(all_nodes.begin(), all_nodes.end());
        for(node_it = all_nodes.begin(); node_it != all_nodes.end(); node_it++){
            Son<W, T> * son = node_it->first.first;
            
            W son_max_wei = son->max_wei;

            if(result.size() >= c_limit && son_max_wei <= result.top().first.first->node->weight) break;

            if (son->node->is_end){
                result.push(*node_it);
                while(result.size() > c_limit) result.pop();
            }

            if(!son->is_arr){
                NodeMap<W, T> * nodeMap = (NodeMap<W, T> *)son->node;
                if (nodeMap->son_next_map->size() != 0 && nodeMap->son_top_vec->size() == 0){
                    for(son_it = nodeMap->son_next_map->begin(); son_it != nodeMap->son_next_map->end(); son_it++){
                        if(son_it->second == NULL)continue;
                        nodeMap->son_top_vec->push_back(std::make_pair(son_it->first, son_it->second->max_wei));
                    }
                    sort(nodeMap->son_top_vec->begin(), nodeMap->son_top_vec->end(), cmp3<uint32_t, W>);
                }
                int j = 0;
                for(top_it = nodeMap->son_top_vec->begin(); top_it != nodeMap->son_top_vec->end(); top_it++){
                    if(queue.size() >= c_limit && top_it->second <= queue.top().first.first->max_wei) break;
                    if(result.size() >= c_limit && top_it->second <= result.top().first.first->node->weight) break;
                    std::string tmp_str = node_it->first.second; U32ToUtf8(top_it->first, tmp_str);
                    queue.push(std::make_pair(std::make_pair((*nodeMap->son_next_map)[top_it->first], tmp_str), node_it->second + 1));
                    while(queue.size() > c_limit) queue.pop();
                    if((++j) > c_limit)throw("error sort");
                }
            }else{
                NodeArr<W, T> * nodeArr = (NodeArr<W, T> *)son->node;
                for(int32_t i = 0; i < nodeArr->son_num; i++){
                    if(nodeArr->all_son[i] == NULL)continue;
                    if(queue.size() >= c_limit && nodeArr->all_son[i]->max_wei <= queue.top().first.first->max_wei) continue;
                    if(result.size() >= c_limit && nodeArr->all_son[i]->max_wei <= result.top().first.first->node->weight) continue;
                    std::string tmp_str = node_it->first.second; U32ToUtf8(nodeArr->all_word[i], tmp_str);
                    queue.push(std::make_pair(std::make_pair(nodeArr->all_son[i], tmp_str), node_it->second + 1));
                    while(queue.size() > c_limit) queue.pop();
                }
            }
            while(queue.size() > c_limit) queue.pop();
            while(result.size() > c_limit) result.pop();
        }
    }
    while(result.size() > c_limit) result.pop();
    while(result.size() != 0){
        std::pair<std::pair<Son<W, T> *, std::string>, int> tmp = result.top();
        ResInfo<W, T> res_info;
        res_info.phrase = tmp.first.second;
        res_info.weight = tmp.first.first->node->weight;
        res_info.info   = tmp.first.first->node->info;
        res_list.push_back(res_info);
        result.pop();
    }
    reverse(res_list.begin(), res_list.end());
}
template<class W, class T, uint32_t top_num>
void Tree<W, T, top_num>::get_suffix_info(const char * content,
                                          std::vector<ResInfo<W, T> > &res_list,
                                          const uint32_t c_limit){
    if(this->root->node == NULL) return;
    Son<W, T> *son = this->get_endp(content);
    if(son == NULL) return;
    this->get_suffix_info(son, content, res_list, c_limit);
}
template<class W, class T, uint32_t top_num>
uint32_t Tree<W, T, top_num>::get_suffix_count(const char *content){
    if(this->root->node == NULL) return 0;
    Son<W, T> *son = this->get_endp(content);
    if(son == NULL) return 0;
    return son->node->in_num;
}
template<class W, class T, uint32_t top_num>
T * Tree<W, T, top_num>::get_info(const char *content){
    if(this->root->node == NULL) return 0;
    Son<W, T> *son = this->get_endp(content);
    if(son == NULL) return NULL;
    return &(son->node->info);
}
template<class W, class T, uint32_t top_num>
NodeMap<W, T> * Tree<W, T, top_num>::arr2map(NodeArr<W, T> *arrNode){
    NodeMap<W, T> *nodemap = new NodeMap<W, T>(arrNode);
    for(int32_t i = 0; i < arrNode->son_num; i++){
        arrNode->all_son[i] = NULL;
    }
    delete arrNode;
    return nodemap;
}




template<class W, class T>
void NodeArr<W, T>::resize(uint8_t new_son_num){
    Son<W, T> **new_all_son = new Son<W, T>*[new_son_num];
    uint32_t *new_all_word = new uint32_t[new_son_num];

    for(int32_t i = 0; i < new_son_num; ++i){
        new_all_son[i] = NULL;
        new_all_word[i] = 0;
    }

    for(int32_t i = 0; i < this->son_num; ++i){
        new_all_son[i] = this->all_son[i];
        this->all_son[i] = NULL;
        new_all_word[i] = this->all_word[i];
    }
    delete all_son;
    delete [] all_word;

    this->son_num = new_son_num;
    this->all_son = new_all_son;
    this->all_word = new_all_word;
}
template<class W, class T>
bool NodeArr<W, T>::resize(std::vector<uint8_t> &arrLen_vec){
    for(int32_t i = 0; i < arrLen_vec.size(); i++){
        if(arrLen_vec[i] > this->son_num){
            this->resize(arrLen_vec[i]);
            return true;
        }
    }
    return false;
}
template<class W, class T>
W NodeArr<W, T>::get_node_max_wei(){
    if(this->in_num <= 0) throw("NodeArr<W, T>::get_node_max_wei there somethig error");
    bool find = false; W w;
    if(this->is_end){
        w = this->weight;
        find = true;
    }
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] != NULL){
            if(!find || this->all_son[i]->max_wei > w){
                w = this->all_son[i]->max_wei;
                find = true;
            }
        }
    }
    return w;
}
template<class W, class T>
void NodeArr<W, T>::set_son(uint32_t word, Son<W, T> *son){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL || this->all_word == NULL)continue;
        if(this->all_word[i] == word){
            this->all_son[i] = son;
            return;
        }
    }
    throw("set_son cant`t find this son");
}
template<class W, class T>
Son<W, T> * NodeArr<W, T>::get_son(uint32_t word){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL)continue;
        if(this->all_word[i] == word) return all_son[i];
    }
    return NULL;
}
template<class W, class T>
Son<W, T> * NodeArr<W, T>::set_son(uint32_t word, W w, uint8_t arrLen){
    for(int32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL){
            all_son[i] = new Son<W, T>();
            all_son[i]->to_arrSon(w, arrLen);
            all_word[i] = word;
            return all_son[i];
        }
    }
    return NULL;
}
template<class W, class T>
Son<W, T> * NodeArr<W, T>::gset_son(uint32_t word, W w, std::vector<uint8_t> &arrLen_vec){
    Son<W, T> *son;
    son = this->get_son(word);
    if(son != NULL) return son;

    son = this->set_son(word, w, arrLen_vec[0]);
    if(son != NULL) return son;

    if(this->resize(arrLen_vec)){
        return this->gset_son(word, w, arrLen_vec);
    }
    return NULL;
}




template<class W, class T>
void NodeMap<W, T>::set_son(uint32_t word, Son<W, T> *son){
    (*this->son_next_map)[word] = son;
}
template<class W, class T>
Son<W, T> * NodeMap<W, T>::get_son(uint32_t word){
    typename std::map<uint32_t, Son<W, T> * >::iterator it = this->son_next_map->find(word);
    if(it != this->son_next_map->end()) return it->second;
    return NULL;
}
template<class W, class T>
Son<W, T> * NodeMap<W, T>::set_son(uint32_t word, W w, uint8_t arrLen){
    Son<W, T> * son = new Son<W, T>();
    son->to_arrSon(w, arrLen);
    (*this->son_next_map)[word] = son;
    return son;
}
template<class W, class T>
Son<W, T> * NodeMap<W, T>::gset_son(uint32_t word, W w, std::vector<uint8_t> &arrLen_vec){
    Son<W, T> *son = this->get_son(word);
    if(son != NULL) return son;
    return this->set_son(word, w, arrLen_vec[0]);
}
template<class W, class T>
W NodeMap<W, T>::get_node_max_wei(){
    if(this->in_num <= 0) throw("NodeMap<W, T>::get_node_max_wei there somethig error");
    bool find = false; W w;
    if(this->is_end){
        w = this->weight;
        find = true;
    }
    typename std::map<uint32_t, Son<W, T> * >::iterator son_it;
    for(son_it = this->son_next_map->begin(); son_it != this->son_next_map->end(); ++son_it){
        if(son_it->second == NULL) continue;
        if(!find || son_it->second->max_wei > w){
            w = son_it->second->max_wei;
            find = true;
        }
    }
    return w;
}
#endif
