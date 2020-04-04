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

template<class W, class T> struct ResInfo{
    std::string phrase;
    uint32_t position;
    uint32_t phrase_len;
    W weight;
    T info;
};

template<class W, class T> class Node{
public:
    virtual Son<W, T> * get_son(uint32_t)=0;

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
        if(this->is_arr){
            delete (NodeArr<W, T> *)this->node;
        }else{
            delete (NodeMap<W, T> *)this->node;
        }
        this->node = NULL;
    }
    virtual Son<W, T> * get_son(uint32_t){}
    virtual Son<W, T> * set_son(uint32_t, W, uint8_t){}
    virtual Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t>&){}
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

         for(uint32_t i = 0; i < nodeArr->son_num; i++){
             if(nodeArr->all_son[i] == NULL) continue;
             (*this->son_next_map)[nodeArr->all_word[i]] = nodeArr->all_son[i];
         }
    }
    ~NodeMap(){
        if(this->son_next_map != NULL){
            typename std::map<uint32_t, Son<W, T> * >::iterator it;
            for(it=this->son_next_map->begin(); it != this->son_next_map->end(); ++it){
                delete it->second;
                it->second = NULL;
            }
            delete this->son_next_map;
            this->son_next_map = NULL;
        }
        if(this->son_top_vec != NULL){
            this->son_top_vec->clear();
            delete this->son_top_vec;
            this->son_top_vec=NULL;
        }
    }

    Son<W, T> * get_son(uint32_t);

    Son<W, T> * set_son(uint32_t, W, uint8_t);

    Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t> &);

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
        for(uint32_t i = 0; i < this->son_num; i++){
            this->all_son[i] = NULL;
            this->all_word[i] = 0;
        }
    }
    ~NodeArr(){
        if(this->all_son != NULL){
            for(uint32_t i = 0; i < this->son_num; i++){
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

    Son<W, T> * get_son(uint32_t);

    Son<W, T> * set_son(uint32_t, W, uint8_t);

    Son<W, T> * gset_son(uint32_t, W, std::vector<uint8_t>&);
public:
    Son<W, T> **all_son;
    uint32_t *all_word;
    uint8_t son_num;
};

template<class W, class T, uint32_t top_num> class Tree{
public:
    Tree(std::vector<uint8_t> arrLen_init_vec, W wei){
        this->arrLen_vec.clear();
        if(arrLen_init_vec.size() != 0){
            for(uint32_t i = 0; i < arrLen_init_vec.size(); i++){
                this->arrLen_vec.push_back(arrLen_init_vec[i]);
            }
        }else{
            this->arrLen_vec.push_back(1);
            this->arrLen_vec.push_back(3);
            this->arrLen_vec.push_back(6);
        }
        this->write_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->query_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        this->qw_cond = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

        pthread_mutex_init(this->write_lock, NULL);
        pthread_cond_init(this->qw_cond, NULL);

        this->root = new Son<W, T>();
        this->root->to_arrSon(wei, this->arrLen_vec[0]);

        this->query_count = 0;
        this->in_update = false;
    }
    ~Tree(){
        free(this->write_lock);
        free(this->query_lock);
        free(this->qw_cond);
        delete this->root;
        this->arrLen_vec.clear();
    }

    NodeMap<W, T> * arr2map(NodeArr<W, T> *);

    bool insert(const char*, W, T, bool force=false);
private:
    W max(W a, W b){
        if(a >= b) return a;
        return b;
    }

public:
    pthread_mutex_t *write_lock;
    pthread_mutex_t *query_lock;
    pthread_cond_t  *qw_cond;
    bool in_update;
    int32_t query_count;
    Son<W, T> *root;
    std::vector<uint8_t> arrLen_vec;
};

template<class W, class T, uint32_t top_num>
bool Tree<W, T, top_num>::insert(const char *content, W weight, T info, bool force){
    Son<W, T> * son = this->root; Son<W, T> ** father = NULL;

    std::vector<uint32_t> candidate_words;
    if(!Utf8ToU32(content, candidate_words) || candidate_words.size() == 0) return false;

    NodeArr<W, T> *nodeArr = NULL; NodeMap<W, T> * nodeMap = NULL; Son<W, T> *next_son = NULL;
    std::vector<uint32_t>:: iterator word_it; 
    std::vector<Son<W, T> *> son_path_vec; son_path_vec.clear();
    for(word_it = candidate_words.begin(); word_it != candidate_words.end(); word_it++){ 
        son_path_vec.push_back(son);
        next_son = son->gset_son(*word_it, weight, this->arrLen_vec);
        if(next_son == NULL){
            son->node = this->arr2map(nodeArr);
            son->is_arr = false;
            next_son = son->gset_son(*word_it, weight, this->arrLen_vec);
        }
        son = next_son;
    }
    if(son->node->is_end && !force)return false;
    for(uint32_t i = 0; i < son_path_vec.size(); i++){
        son_path_vec[i]->max_wei = this->max(son_path_vec[i]->max_wei, weight);
        son_path_vec[i]->node->in_num += 1;
    }
    son->node->is_end = true;
    son->node->weight = weight;
    son->node->in_num += 1;
    son->node->info = info;
}


template<class W, class T, uint32_t top_num>
bool Tree<W, T, top_num>::get_endp(std::vector<uint32_t>:: iterator start,
                                   std::vector<uint32_t>:: iterator end){
    Son<W, T> *son = Tree->root;
    for(vector<uint32_t>:: iterator it = start; it != end; it++){
        Son<W, T> * next_son = son->get_son(*it);
        if(next_son == NULL) return false;
        son = next_son;
    }
    return son;
}

struct cmp1 { //从大到小
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        if (left.first.second < right.first.second) return true;
        return false;
    }
};
struct cmp2 { //从小到大
    template <typename T, typename U>
    bool operator()(T const &left, U const &right) {
        if (left.first.second > right.first.second) return true;
        return false;
    }
};
bool cmp4(const pair<uint32_t, int> a, const pair<uint32_t, int> b) {
    return a.second > b.second; /// 大的在前
};

template<class W, class T, uint32_t top_num>
void Tree<W, T, top_num>::get_suffix_pinfo(const string prefix_str,
                                           vector<ResInfo *> &res_list,
                                           int c_limit=(1<<30),
                                           int d_limit=(1<<30)
                                           ){
    priority_queue<pair<Son<W, T> *, string>, vector<pair<Son<W, T> *, string> >, cmp2>queue;
    while(!queue.empty()) queue.pop();
    priority_queue<pair<Son<W, T> *, string>, vector<pair<Son<W, T> *, string> >, cmp2>result;
    while(!queue.empty()) queue.pop();

    queue.push(make_pair(this->root, string(prefix_str)));

    vector<pair<Son<W, T> *, string> > all_nodes; Son<W, T> * son = NULL; int dep = 0;
    while(queue.size()){
        dep += 1; all_nodes.clear();
        while(queue.size()){
            all_nodes.push_back(queue.top()); queue.pop();
        }
        reverse(all_nodes.begin(), all_nodes.end());
        for(vector<pair<Son<W, T>, string> >::iterator it = all_nodes.begin(); it != all_nodes.end(); it++){
            son = it->first;
            int weight = son->weight;
            if(dep > d_limit) continue;

            if(result.size() >= c_limit && weight <= result.top().first->weight) break;

            if (son->is_end){
                result.push(make_pair(son, is->second));
                while(result.size() > c_limit) result.pop();
            }
            if (p->type == 0){
                Nodes* pn = (Nodes *)p->next;
                if (pn->next_map->size() != 0 && pn->next_top_wei->size() == 0){
                    for(map<uint32_t, pair<Next *, int> >::iterator it = pn->next_map->begin(); it != pn->next_map->end(); it++){
                        if(it->second.first != NULL){
                            pn->next_top_wei->push_back(make_pair(it->first, it->second.second));
                        }
                    }
                    sort(pn->next_top_wei->begin(), pn->next_top_wei->end(), cmp4);
                }
                int j = 0;
                for(vector<pair<uint32_t, int> >::iterator it = pn->next_top_wei->begin(); it != pn->next_top_wei->end(); it++){
                    if(d_limit == (1<<30) && queue.size() >= c_limit && it->second <= queue.top().first.second) break;
                    if(result.size() >= c_limit && it->second <= result.top().first.second) break;
                    string tmp_str = res_str; Unicode32ToUtf8(it->first, tmp_str);
                    queue.push(make_pair(make_pair((*pn->next_map)[it->first].first, it->second), make_pair(tmp_str, dep + 1)));
                    while(d_limit == (1<<30) && queue.size() > c_limit) queue.pop();
                    if(d_limit == (1<<30) && (++j) > c_limit)throw("error sort");
                }
            }else if (p->type == 1){
                Node_1* p1 = (Node_1 *)p->next;
                if(p1->next == NULL) continue;
                if(d_limit == (1<<30) && queue.size() >= c_limit && p1->max_wei <= queue.top().first.second) continue;
                if(result.size() >= c_limit && p1->max_wei <= result.top().first.second) continue;
                string tmp_str = res_str; Unicode32ToUtf8(p1->word, tmp_str);
                queue.push(make_pair(make_pair(p1->next, p1->max_wei), make_pair(tmp_str, dep + 1)));
                while(d_limit == (1<<30) && queue.size() > c_limit) queue.pop();
            }else if (p->type == 3){
                Node_3* p3 = (Node_3 *)p->next;
                for(uint32_t i = 0; i < 3; i++){
                    if(p3->nexts[i] == NULL) continue;
                    if(d_limit == (1<<30) && queue.size() >= c_limit && p3->max_wei[i] <= queue.top().first.second) continue;
                    if(result.size() >= c_limit && p3->max_wei[i] <= result.top().first.second) continue;
                    string tmp_str = res_str; Unicode32ToUtf8(p3->words[i], tmp_str);
                    queue.push(make_pair(make_pair(p3->nexts[i], p3->max_wei[i]), make_pair(tmp_str, dep + 1)));
                    while(d_limit == (1<<30) && queue.size() > c_limit) queue.pop();
                }
            } else if (p->type == 6){
                Node_6* p6 = (Node_6 *)p->next;
                for(uint32_t i = 0; i < 6; i++){
                    if(p6->nexts[i] == NULL) continue;
                    if(d_limit == (1<<30) && queue.size() >= c_limit && p6->max_wei[i] <= queue.top().first.second) continue;
                    if(result.size() >= c_limit && p6->max_wei[i] <= result.top().first.second) continue;
                    string tmp_str = res_str; Unicode32ToUtf8(p6->words[i], tmp_str);
                    queue.push(make_pair(make_pair(p6->nexts[i], p6->max_wei[i]), make_pair(tmp_str, dep + 1)));
                    while(d_limit == (1<<30) && queue.size() > c_limit) queue.pop();
                }
            } else{
                throw("next->type not in (0, 1, 3, 6)");
            }
            while(d_limit == (1<<30) && queue.size() > c_limit) queue.pop();
            while(result.size() > c_limit) result.pop();
        }
    }
    while(result.size() > c_limit) result.pop();
    while(result.size() != 0){
        pair<pair<Next *, int>, pair<string, int> > node = result.top(); result.pop();
        ResInfo *res_info = new ResInfo();
        res_info->phrase = node.second.first;
        res_info->weight = node.first.second;
        res_info->oinfo = get_node_oinfo(node.first.first);
        res_list.push_back(res_info);
    }
    reverse(res_list.begin(), res_list.end());
}


template<class W, class T, uint32_t top_num>
NodeMap<W, T> * Tree<W, T, top_num>::arr2map(NodeArr<W, T> *arrNode){
    NodeMap<W, T> *nodemap = new NodeMap<W, T>(arrNode);
    for(uint32_t i = 0; i < arrNode->son_num; i++){
        arrNode->all_son[i] = NULL;
    }
    //arrNode->all_son = NULL;
    delete arrNode;
    return nodemap;
}

template<class W, class T>
void NodeArr<W, T>::resize(uint8_t new_son_num){
    //std::cout<<int(new_son_num)<<" "<<int(this->son_num)<<std::endl;
    Son<W, T> **new_all_son = new Son<W, T>*[new_son_num];
    uint32_t *new_all_word = new uint32_t[new_son_num];

    for(uint32_t i = 0; i < new_son_num; ++i){
        new_all_son[i] = NULL;
        new_all_word[i] = 0;
    }

    for(uint32_t i = 0; i < this->son_num; ++i){
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
    for(uint32_t i = 0; i < arrLen_vec.size(); i++){
        if(arrLen_vec[i] > this->son_num){
            this->resize(arrLen_vec[i]);
            return true;
        }
    }
    return false;
}

template<class W, class T>
Son<W, T> * NodeArr<W, T>::get_son(uint32_t word){
    for(uint32_t i = 0; i < this->son_num; i++){
        if(this->all_son[i] == NULL || this->all_word == NULL)continue;
        if(this->all_word[i] == word) return all_son[i];
    }
    return NULL;
}
template<class W, class T>
Son<W, T> * NodeArr<W, T>::set_son(uint32_t word, W w, uint8_t arrLen){
    for(uint32_t i = 0; i < this->son_num; i++){
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
#endif
