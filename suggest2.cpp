# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<unistd.h>
# include<string.h>
# include<map>
# include<vector>
# include<time.h>
# include<stdint.h>
# include<queue>
# include<algorithm>
# include<queue>
# include<utility>
# include<pthread.h>
#include <sys/time.h>
//# include<mutex>
using namespace std;

string getTodayDate(int add_days=0, string format="%Y-%m-%d %H:%M:%S") {
    time_t timep;
    time (&timep);
    char tmp[64];
    timep = timep + 3600 * 24 * add_days;
    strftime(tmp, sizeof(tmp), format.c_str(), localtime(&timep));
    return tmp;
}

uint64_t get_micron_second(){
      struct timeval tv;
      struct timezone tz;
      gettimeofday(&tv,&tz);
      return tv.tv_sec*1000000 +tv.tv_usec;
}

bool Utf8ToUnicode32(const string& str, vector<uint32_t>& vec) {
  uint32_t tmp; vec.clear();
  for(size_t i = 0; i < str.size();) {
    if(!(str[i] & 0x80)) { // 0xxxxxxx
      // 7bit, total 7bit
      tmp = (uint8_t)(str[i]) & 0x7f;
      i++;
    } else if ((uint8_t)str[i] <= 0xdf && i + 1 < str.size()) { // 110xxxxxx
      // 5bit, total 5bit
      tmp = (uint8_t)(str[i]) & 0x1f;

      // 6bit, total 11bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;
      i += 2;
    } else if((uint8_t)str[i] <= 0xef && i + 2 < str.size()) { // 1110xxxxxx
      // 4bit, total 4bit
      tmp = (uint8_t)(str[i]) & 0x0f;

      // 6bit, total 10bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;

      // 6bit, total 16bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+2]) & 0x3f;

      i += 3;
    } else if((uint8_t)str[i] <= 0xf7 && i + 3 < str.size()) { // 11110xxxx
      // 3bit, total 3bit
      tmp = (uint8_t)(str[i]) & 0x07;

      // 6bit, total 9bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;

      // 6bit, total 15bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+2]) & 0x3f;

      // 6bit, total 21bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+3]) & 0x3f;

      i += 4;
    } else {
      return false;
    }
    vec.push_back(tmp);
  }
  return true;
}


void Unicode32ToUtf8(const uint32_t &ui, string& res) {
  if(ui <= 0x7f) {
    res += char(ui);
  } else if(ui <= 0x7ff) {
    res += char(((ui >> 6) & 0x1f) | 0xc0);
    res += char((ui & 0x3f) | 0x80);
  } else if(ui <= 0xffff) {
    res += char(((ui >> 12) & 0x0f) | 0xe0);
    res += char(((ui >> 6) & 0x3f) | 0x80);
    res += char((ui & 0x3f) | 0x80);
  } else {
    res += char(((ui >> 18) & 0x03) | 0xf0);
    res += char(((ui >> 12) & 0x3f) | 0x80);
    res += char(((ui >> 6) & 0x3f) | 0x80);
    res += char((ui & 0x3f) | 0x80);
  }
}


void PrintUnicode(const uint32_t &ui) {
  string res = ""; Unicode32ToUtf8(ui, res);
  cout<<res<<endl;
}


char * get_random_str(int len){
    char * p = (char *) malloc(sizeof(char *) * (len + 1));
    for(int i = 0; i < len; i++) {
        int s = rand() % 2;                     //随机使s为1或0，为1就是大写，为0就是小写 
        if(s == 1)                        //如果s=1 
            p[i] = rand() % ('Z'-'A'+1) + 'A';       //将x赋为大写字母的ascii码 
        else 
            p[i] = rand() % ('z'-'a'+1) + 'a';       //如果s=0，x赋为小写字母的ascii码 
    }
    p[len] = '\0';
    return p;
}

struct Next{
    void *next;
    uint8_t type;
};
struct Tree{
    pthread_mutex_t *up_lock;
    pthread_mutex_t *up_lock2;
    pthread_cond_t *up_cond;
    bool in_update;
    int query_count;
    Next * root;
    string str;
};
struct OInfo{
    void *p;
    int8_t type;
};
struct Node_1{
    bool is_end;
    uint32_t word;
    int weight;
    int max_wei;
    int in;
    Next *next;
    OInfo *oinfo;
};
struct Node_3{
    bool is_end;
    uint32_t words[3];
    int max_wei[3];
    Next *nexts[3];
    int weight;
    int in;
    OInfo *oinfo;
};
struct Node_6{
    bool is_end;
    uint32_t words[6];
    int max_wei[6];
    Next *nexts[6];
    int weight;
    int in;
    OInfo *oinfo;
};
struct Nodes{
    bool is_end;
    map<uint32_t, pair<Next *, int> > *next_map;
    vector<pair<uint32_t, int> > *next_top_wei;
    int weight;
    int in;
    OInfo *oinfo;
};
bool get_node_end(Next *p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        return pn->is_end;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        return p1->is_end;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        return p3->is_end;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        return p6->is_end;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
int get_node_in(Next *p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        return pn->in;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        return p1->in;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        return p3->in;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        return p6->in;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
const OInfo * get_node_oinfo(Next *p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        return pn->oinfo;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        return p1->oinfo;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        return p3->oinfo;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        return p6->oinfo;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
void update_node_end(Next *p, bool is_end){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        pn->is_end = is_end;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        p1->is_end = is_end;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        p3->is_end = is_end;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        p6->is_end = is_end;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
void update_node_weight(Next *p, int weight){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        pn->weight = weight;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        p1->weight = weight;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        p3->weight = weight;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        p6->weight = weight;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
int get_node_weight(Next *p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        return pn->weight;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        return p1->weight;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        return p3->weight;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        return p6->weight;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
void update_node_next_max_wei(Next *p, int word, int weight){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        (*pn->next_map)[word].second = weight;
        pn->next_top_wei->clear();
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->word == word){
            p1->max_wei = weight;
        }else{
            throw("update_node_weight_1 error");
        }
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        for(int i = 0; i < 3; i++){
            if(p3->words[i] == word){
                p3->max_wei[i] = weight;
                return;
            }
        }
        throw("update_node_weight_3 error");
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        for(int i = 0; i < 6; i++){
            if(p6->words[i] == word){
                p6->max_wei[i] = weight;
                return;
            }
        }
        throw("update_node_weight_6 error");
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
int get_node_next_max_wei(Next *p, int word){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        return (*pn->next_map)[word].second;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->word == word) return p1->max_wei;
        throw("update_node_weight_1 error");
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        for(int i = 0; i < 3; i++){
            if(p3->words[i] == word) return p3->max_wei[i];
        }
        throw("update_node_weight_3 error");
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        for(int i = 0; i < 6; i++){
            if(p6->words[i] == word) return p6->max_wei[i];
        }
        throw("update_node_weight_6 error");
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
    return -1;
}
int get_node_max_weight(Next *p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    int max_weight = -(1<<30);
    if(p->type == 0){
        pn = (Nodes *)p->next;
        max_weight = max(max_weight, pn->weight);
        for(map<uint32_t, pair<Next *, int> >::iterator it = pn->next_map->begin(); it != pn->next_map->end(); it++){
            max_weight = max(it->second.second, max_weight);
        }
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        max_weight = max(max_weight, p1->weight);
        max_weight = max(p1->max_wei, max_weight);
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        max_weight = max(max_weight, p3->weight);
        for(int i = 0; i < 3; i++){
            max_weight = max(p3->max_wei[i], max_weight);
        }
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        max_weight = max(max_weight, p6->weight);
        for(int i = 0; i < 6; i++){
            max_weight = max(p6->max_wei[i], max_weight);
        }
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
    return max_weight;
}
void increase_node_in(Next *p, int num){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        pn->in += num;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        p1->in += num;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        p3->in += num;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        p6->in += num;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}
void update_node_oinfo(Next *p, OInfo * oinfo, bool force=false){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        if(pn->oinfo != NULL){
            if(pn->oinfo->type == 0){
                pn->oinfo->p = NULL;
            }
            else if(pn->oinfo->type == 1){
                free(pn->oinfo->p);
                pn->oinfo->p = NULL;
            }else if(force){
                pn->oinfo = NULL;
            }else{
                throw("update_node_oinfo  only suport type in (0, 1)");
            }
        }
        free(pn->oinfo);
        pn->oinfo = oinfo;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->oinfo != NULL){
            if(p1->oinfo->type == 0){
                p1->oinfo->p = NULL;
            }
            else if(p1->oinfo->type == 1){
                free(p1->oinfo->p);
                p1->oinfo->p = NULL;
            }else if(force){
                p1->oinfo = NULL;
            }else{
                throw("update_node_oinfo  only suport type in (0, 1)");
            }
        }
        free(p1->oinfo);
        p1->oinfo = oinfo;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        if(p3->oinfo != NULL){
            if(p3->oinfo->type == 0){
                p3->oinfo->p = NULL;
            }
            else if(p3->oinfo->type == 1){
                free(p3->oinfo->p);
                p3->oinfo->p = NULL;
            }else if(force){
                p3->oinfo = NULL;
            }else{
                throw("update_node_oinfo  only suport type in (0, 1)");
            }
        }
        free(p3->oinfo);
        p3->oinfo = oinfo;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        if(p6->oinfo != NULL){
            if(p6->oinfo->type == 0){
                p6->oinfo->p = NULL;
            }
            else if(p6->oinfo->type == 1){
                free(p6->oinfo->p);
                p6->oinfo->p = NULL;
            }else if(force){
                p6->oinfo = NULL;
            }else{
                throw("update_node_oinfo  only suport type in (0, 1)");
            }
        }
        free(p6->oinfo);
        p6->oinfo = oinfo;
    }else{
        throw("next->type is not in (0, 1, 3, 6)");
    }
}


OInfo * get_oinfo(void *p, uint8_t type){
    OInfo *oinfo = (OInfo *)malloc(sizeof(OInfo));
    oinfo->p = p;
    oinfo->type = type;
    return oinfo;
}
Node_1 * get_node_1(){
    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
    tmp_node->is_end = false;
    tmp_node->word = 0;
    tmp_node->max_wei = 0;
    tmp_node->next = NULL;

    tmp_node->weight = 0;
    tmp_node->oinfo = NULL;
    tmp_node->in = 0;
    return tmp_node;
}
Node_3 * get_node_3(){
    Node_3 * new_node = (Node_3 *)malloc(sizeof(Node_3));
    new_node->is_end = false;
    for(unsigned int i = 0; i < 3; i ++){
        new_node->words[i] = 0;
        new_node->nexts[i] = NULL;
        new_node->max_wei[i] = 0;
    }
    new_node->weight = 0;
    new_node->in = 0;
    new_node->oinfo = NULL;
    return new_node;
}
Node_6 * get_node_6(){
    Node_6 * new_node = (Node_6 *)malloc(sizeof(Node_6));
    new_node->is_end = false;
    for(unsigned int i = 0; i < 6; i ++){
        new_node->words[i] = 0;
        new_node->nexts[i] = NULL;
        new_node->max_wei[i] = 0;
    }
    new_node->weight = 0;
    new_node->in = 0;
    new_node->oinfo = NULL;
    return new_node;
}
Nodes * get_nodes(){
    Nodes * new_node = (Nodes *)malloc(sizeof(Nodes));
    new_node->is_end = false;
    new_node->next_map = new map<uint32_t, pair<Next *, int> >;
    new_node->next_top_wei = new vector<pair<uint32_t, int> >;
    new_node->next_map->clear();
    new_node->next_top_wei->clear();

    new_node->weight = 0;
    new_node->in = 0;
    new_node->oinfo = NULL;
    return new_node;
}


void dfs_update_father_weight(vector<Next *>   &vec_path,
                              vector<uint32_t> &word_path,
                              int index,
                              int weight){
    if(index < 0) return;
    Next * fat = vec_path[index];
    int fat_word = word_path[index];
    int fat_wei = get_node_next_max_wei(fat, fat_word);
    if(fat_wei >= weight) return;
    update_node_next_max_wei(fat, fat_word, weight);
    int son_weight = get_node_max_weight(fat);
    if(index > 0) dfs_update_father_weight(vec_path,
                                           word_path,
                                           index - 1,
                                           son_weight);
}


bool single_insert(Tree *, const char *, OInfo *, int);
bool insert(Tree * tree, const char* content, OInfo *oinfo, int weight=0){
    pthread_mutex_lock(tree->up_lock2);
    tree->in_update = true;
    pthread_mutex_lock(tree->up_lock);

    try{
        while(tree->query_count > 0){
            pthread_cond_wait(tree->up_cond, tree->up_lock);
        }
        cout<<"000000000001"<<" "<<tree->query_count<<" "<<get_micron_second()<<" "<<time(NULL)<<endl;
        bool res = single_insert(tree, content, oinfo, weight);
        tree->in_update = false;
        cout<<"000000000002"<<" "<<tree->query_count<<" "<<get_micron_second()<<" "<<time(NULL)<<endl;
        pthread_mutex_unlock(tree->up_lock);
        pthread_mutex_unlock(tree->up_lock2);
        pthread_cond_broadcast(tree->up_cond);
        return res;
    }catch(...){
        tree->in_update = false;
        pthread_mutex_unlock(tree->up_lock);
        pthread_mutex_unlock(tree->up_lock2);
        pthread_cond_broadcast(tree->up_cond);
        throw("single_insert error string=" + string(content));
    }
}

bool single_insert(Tree* tree, const char* content, OInfo *oinfo, int weight=0){
    int dep = 0; Next* p = tree->root; Next** parent = &(tree->root);

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, pair<Next *, int> >::iterator ik;
    vector<Next *> vec_path; vec_path.clear();
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++, dep++){
        //printf("%d, %d\n", p->type, *it);
        vec_path.push_back(p);
        if (p->type == 0){
            pn = (Nodes *)p->next;
            pn->in += 1;
            ik = pn->next_map->find(*it);
            if(ik != pn->next_map->end()){
                parent = &ik->second.first;
                p = ik->second.first;
                (*pn->next_map)[*it].second = max((*pn->next_map)[*it].second, weight);
            }else{
                Next * tmp_next = (Next *)malloc(sizeof(Next));
                tmp_next->next = get_node_1();
                tmp_next->type = 1;

                (*pn->next_map)[*it] = make_pair(tmp_next, weight);
                parent = &tmp_next;
                p = tmp_next;
            }
            pn->next_top_wei->clear();
        }else if(p->type == 1){
            p1 = (Node_1 * )p->next;
            p1->in += 1;
            if(p1->next != NULL && p1->word == *it){
                p1->max_wei = max(p1->max_wei, weight);
                parent = &(p1->next);
                p = p1->next;
            }else{
                if(p1->next == NULL){
                    p1->next = (Next *)malloc(sizeof(Next));
                    p1->next->next = get_node_1();
                    p1->next->type = 1;
                    p1->max_wei = weight;
                    p1->word = *it;

                    parent = &(p1->next);
                    p = p1->next;
                }else{
                    Node_3 * new_node = get_node_3();
                    new_node->words[0] = p1->word;
                    new_node->nexts[0] = p1->next;
                    new_node->max_wei[0] = p1->max_wei;

                    new_node->nexts[1] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[1]->next = get_node_1();

                    new_node->nexts[1]->type = 1;
                    new_node->words[1] = *it;
                    new_node->max_wei[1] = weight;

                    new_node->weight = p1->weight;
                    new_node->is_end = p1->is_end;
                    new_node->in = p1->in;
                    new_node->oinfo = p1->oinfo;

                    (*parent)->next = new_node;
                    (*parent)->type = 3;
                    free(p1);

                    parent = &(new_node->nexts[1]);
                    p = new_node->nexts[1];
                }
            }
        }else if(p->type == 3){
            //printf("%d, tt\n" , root->type);
            p3 = (Node_3 * )p->next;
            p3->in += 1;
            bool is_find = false;
            for(unsigned int i = 0; i < 3; i++){
                if(p3->words[i] == *it){
                    is_find = true;
                    p3->max_wei[i] = max(p3->max_wei[i], weight);
                    parent = &(p3->nexts[i]);
                    p = p3->nexts[i];
                    break;
                }
            }
            if(!is_find){
                bool find_position = false;
                for(unsigned int i = 0; i < 3; i++){
                    if(p3->nexts[i] != NULL) continue;
                    p3->nexts[i] = (Next *)malloc(sizeof(Next));
                    p3->nexts[i]->next = get_node_1();
                    p3->nexts[i]->type = 1;
                    p3->words[i] = *it;
                    p3->max_wei[i] = weight;

                    parent = &(p3->nexts[i]);
                    p = p3->nexts[i];
                    find_position = true;
                    break;
                }
                if(!find_position){
                    Node_6 * new_node = get_node_6();

                    for(unsigned int i = 0; i < 3; i++){
                        new_node->words[i] = p3->words[i];
                        new_node->nexts[i] = p3->nexts[i];
                        new_node->max_wei[i] = p3->max_wei[i];
                    }
                    new_node->nexts[3] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[3]->next = get_node_1();

                    new_node->nexts[3]->type = 1;
                    new_node->max_wei[3] = weight;
                    new_node->words[3] = *it;

                    new_node->in = p3->in;
                    new_node->weight = p3->weight;
                    new_node->is_end = p3->is_end;
                    new_node->oinfo = p3->oinfo;

                    (*parent)->next = new_node;
                    (*parent)->type = 6;
                    free(p3);

                    parent = &(new_node->nexts[3]);
                    p = new_node->nexts[3];
                }
            }
        }else if(p->type == 6){
            p6 = (Node_6 * )p->next;
            p6->in += 1;
            bool is_find = false;
            for(unsigned int i = 0; i < 6; i++){
                if(p6->words[i] == *it){
                    is_find = true;
                    p6->max_wei[i] = max(p6->max_wei[i], weight);
                    parent = &(p6->nexts[i]);
                    p = p6->nexts[i];
                    break;
                }
            }
            if(!is_find){
                bool have_position = false;
                for(unsigned int i = 0; i < 6; i++){
                    if(p6->nexts[i] != NULL) continue;
                    p6->nexts[i] = (Next *)malloc(sizeof(Next));
                    p6->nexts[i]->next = get_node_1();
                    p6->nexts[i]->type = 1;
                    p6->words[i] = *it;
                    p6->max_wei[i] = weight;

                    parent = &(p6->nexts[i]);
                    p = p6->nexts[i];
                    have_position = true;
                    break;
                }
                if(!have_position){
                    Nodes * new_node = get_nodes();

                    Next *tt_next = (Next *)malloc(sizeof(Next));
                    tt_next->next = get_node_1();
                    tt_next->type = 1;

                    for(unsigned int i = 0; i < 6; i++){
                        (*new_node->next_map)[p6->words[i]] = make_pair(p6->nexts[i], p6->max_wei[i]);
                    }
                    (*new_node->next_map)[*it] = make_pair(tt_next, weight);
                    new_node->in = p6->in;
                    new_node->is_end = p6->is_end;
                    new_node->weight = p6->weight;
                    new_node->oinfo = p6->oinfo;

                    (*parent)->next = new_node;
                    (*parent)->type = 0;
                    free(p6);

                    parent = &tt_next;
                    p = tt_next;
                }
            }
        }else{
            throw("找不到指针的类型");
        }
        //printf("%d, %d, %d\n", dep, (*parent)->type, *it);
    }
    bool res = get_node_end(p);
    if(res){
        const OInfo *info = get_node_oinfo(p);
        if(oinfo == NULL || (oinfo->type != 0 && oinfo->type != 1)){
            for(int i = 0; i < vec_path.size(); i++){
                increase_node_in(vec_path[i], -1);
            }
            return false;
        }
    }
    if(res){
        for(int i = 0; i < vec_path.size(); i++){
            increase_node_in(vec_path[i], -1);
        }
        if(vec_path.size() != 0){
            increase_node_in(p, -1);
        }
        update_node_weight(p, weight);
        int son_weight = get_node_max_weight(p);
        if(vec_path.size() >= 1)
            dfs_update_father_weight(vec_path,
                                     candidate_words,
                                     vec_path.size() - 1,
                                     son_weight);
    }
    increase_node_in(p, 1);
    update_node_oinfo(p, oinfo);
    update_node_weight(p, weight);
    update_node_end(p, true);
    return true;
}

bool single_remove(Tree *, const char *, bool);
bool remove(Tree * tree, const char* content, bool force=false){
    pthread_mutex_lock(tree->up_lock);
    try{
        while(tree->query_count > 0){
            pthread_cond_wait(tree->up_cond, tree->up_lock);
        }
        tree->in_update = true;
        bool res = single_remove(tree, content, force);
        tree->in_update = false;
        pthread_mutex_unlock(tree->up_lock);
        pthread_cond_broadcast(tree->up_cond);
        return res;
    }catch(...){
        tree->in_update = false;
        pthread_mutex_unlock(tree->up_lock);
        pthread_cond_broadcast(tree->up_cond);
        throw("single_remove error string=" + string(content));
    }
}
bool single_remove(Tree * tree, const char* content, bool force=false){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    map<uint32_t, pair<Next *, int> >::iterator ik; Next* p = tree->root;
    vector<Next *> vec_path; vec_path.clear();
    for(vector<uint32_t>::iterator it = candidate_words.begin(); it != candidate_words.end(); it++){
        vec_path.push_back(p);
        if(p->type == 0){
           pn = (Nodes *)p->next;
            if(pn->next_map != NULL){
                ik = pn->next_map->find(*it);
                if(ik != pn->next_map->end()){
                    p = ik->second.first;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }else if(p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->next != NULL && p1->word == *it){
                p = p1->next;
            }else{
                return false;
            }
        }else if(p->type == 3){
            p3 = (Node_3 *)p->next;
            bool find = false;
            for(int i = 0; i < 3; i++){
                if(p3->nexts[i] != NULL && p3->words[i] == *it){
                    p = p3->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return false;
        }else if(p->type == 6){
            p6 = (Node_6 *)p->next;
            bool find = false;
            for(int i = 0; i < 6; i++){
                if(p6->nexts[i] != NULL && p6->words[i] == *it){
                    p = p6->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return false;
        }else{
            throw("next->type not in (0, 1, 3, 6)");
        }
    }

    bool res = get_node_end(p);
    if(res && !force){
        const OInfo * oinfo = get_node_oinfo(p);
        if(oinfo == NULL || (oinfo->type != 0 && oinfo->type != 1)){
            for(int i = 0; i < vec_path.size(); i++){
                increase_node_in(vec_path[i], -1);
            }
            return false;
        }
    }
    if(res){
        for(int i = 0; i < vec_path.size(); i++){
            increase_node_in(vec_path[i], -1);
        }
        if(vec_path.size() != 0){
            increase_node_in(p, -1);
        }
        update_node_weight(p, 0);
        int son_weight = get_node_max_weight(p);
        if(vec_path.size() >= 1)
            dfs_update_father_weight(vec_path,
                                     candidate_words,
                                     vec_path.size() - 1,
                                     son_weight);
    }
    update_node_oinfo(p, NULL, force);
    update_node_end(p, false);
    update_node_weight(p, 0);
    return res;
}


struct ResInfo{
    string phrase;
    uint32_t position;
    uint32_t phrase_len;
    int weight;
    const OInfo *oinfo;
};
ResInfo * get_resinfo(){
    ResInfo * t = new ResInfo();
    t->phrase = "";
    t->position = 0;
    t->phrase_len = 0;
    t->oinfo = NULL;
    t->weight = 0;
    return t;
}
int32_t cut_from(Tree * tree,
                 vector<uint32_t>:: iterator start,
                 vector<uint32_t>:: iterator end,
                 vector<ResInfo *> &res_list,
                 int32_t offset_position=0){
    Next* p = tree->root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, pair<Next *, int> >::iterator ik;
    string res_str = "";
    int32_t max_position = -1;
    for(vector<uint32_t>:: iterator it = start; it != end; it++, offset_position++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = ik->second.first;
            if(get_node_end(p)){
                ResInfo *res_info = new ResInfo();
                res_info->phrase = res_str;
                res_info->position = offset_position;
                res_info->oinfo = get_node_oinfo(p);
                res_list.push_back(res_info);
                max_position = max(max_position, offset_position);
            }
            continue;
        }
        if (p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->word != *it)return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = p1->next;
            if(get_node_end(p)){
                ResInfo *res_info = new ResInfo();
                res_info->phrase = res_str;
                res_info->position = offset_position;
                res_info->oinfo = get_node_oinfo(p);
                res_list.push_back(res_info);
                max_position = max(max_position, offset_position);
            }
            continue;
        }
        if (p->type == 3){
            p3 = (Node_3 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 3; i++){
                if(p3->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p3->nexts[i];
                    if(get_node_end(p)){
                        ResInfo *res_info = new ResInfo();
                        res_info->phrase = res_str.c_str();
                        res_info->position = offset_position;
                        res_info->oinfo = get_node_oinfo(p);
                        res_list.push_back(res_info);
                        max_position = max(max_position, offset_position);
                    }
                    find = true;
                    break;
                }
            }
            if(!find) return max_position;
            continue;
        }
        if (p->type == 6){
            p6 = (Node_6 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 6; i++){
                if(p6->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p6->nexts[i];
                    if(get_node_end(p)){
                        ResInfo *res_info = new ResInfo();
                        res_info->phrase = res_str;
                        res_info->position = offset_position;
                        res_info->oinfo = get_node_oinfo(p);
                        res_list.push_back(res_info);
                        max_position = max(max_position, offset_position);
                    }
                    find = true;
                    break;
                }
            }
            if(!find) return max_position;
            continue;
        }
    }
    return max_position;
}
/*
return: 从当前位置切分的最长切分位置
*/
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
//bool cmp3(const pair<pair<Next *, int>, pair<string, int> > a, const pair<pair<Next *, int>, pair<string, int> > b) {
//    return a.first.second > b.first.second; /// 大的在前
//};
bool cmp4(const pair<uint32_t, int> a, const pair<uint32_t, int> b) {
    return a.second > b.second; /// 大的在前
};
void get_tree_suffix_phrases(Next * p,
                             const string prefix_str,
                             vector<ResInfo *> &res_list,
                             int c_limit=(1<<30),
                             int d_limit=(1<<30)
                             ){
    priority_queue<pair<pair<Next *, int>, pair<string, int> >, vector<pair<pair<Next *, int>, pair<string, int> > >, cmp2>queue;
    while(!queue.empty()) queue.pop();
    priority_queue<pair<pair<Next *, int>, pair<string, int> >, vector<pair<pair<Next *, int>, pair<string, int> > >, cmp2>result;
    while(!queue.empty()) queue.pop();
    queue.push(make_pair(make_pair(p, get_node_weight(p)), make_pair(string(prefix_str), 0)));

    vector<pair<pair<Next *, int>, pair<string, int> > > all_nodes;
    while(queue.size()){
        all_nodes.clear();
        while(queue.size()){
            all_nodes.push_back(queue.top()); queue.pop();
        }
        reverse(all_nodes.begin(), all_nodes.end());
        for(vector<pair<pair<Next *, int>, pair<string, int> > >::iterator it = all_nodes.begin(); it != all_nodes.end(); it++){
            Next *p = it->first.first;
            int weight = it->first.second;
            string res_str = it->second.first;
            int dep = it->second.second;
            if(dep > d_limit) continue;

            if(result.size() >= c_limit && weight <= result.top().first.second) break;

            if (get_node_end(p)){
                result.push(make_pair(make_pair(p, get_node_weight(p)), make_pair(res_str, dep)));
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
Next * get_phrase_endp(Tree * tree,
                       vector<uint32_t>:: iterator start,
                       vector<uint32_t>:: iterator end
                       ){
    Next* p = tree->root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, pair<Next *, int> >::iterator ik;
    for(vector<uint32_t>:: iterator it = start; it != end; it++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return NULL;
            p = ik->second.first;
            continue;
        }
        if (p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->word != *it)return NULL;
            p = p1->next;
            continue;
        }
        if (p->type == 3){
            p3 = (Node_3 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 3; i++){
                if(p3->words[i] == *it){
                    p = p3->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return NULL;
            continue;
        }
        if (p->type == 6){
            p6 = (Node_6 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 6; i++){
                if(p6->words[i] == *it){
                    p = p6->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return NULL;
            continue;
        }
    }
    return p;
}
Next * get_lcp_endp(Tree * tree,
                   string &res_str,
                   vector<uint32_t>:: iterator start,
                   vector<uint32_t>:: iterator end
                   ){
    Next* p = tree->root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, pair<Next *, int> >::iterator ik;
    for(vector<uint32_t>:: iterator it = start; it != end; it++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return p;
            Unicode32ToUtf8(*it, res_str);
            p = ik->second.first;
            continue;
        }
        if (p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->word != *it)return p;
            p = p1->next;
            Unicode32ToUtf8(*it, res_str);
            continue;
        }
        if (p->type == 3){
            p3 = (Node_3 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 3; i++){
                if(p3->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p3->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return p;
            continue;
        }
        if (p->type == 6){
            p6 = (Node_6 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 6; i++){
                if(p6->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p6->nexts[i];
                    find = true;
                    break;
                }
            }
            if(!find) return p;
            continue;
        }
    }
    return p;
}

void single_free_root(Next *, bool);
void free_tree(Tree * tree, bool force=false){
    pthread_mutex_lock(tree->up_lock);
    try{
        while(tree->query_count > 0){
            pthread_cond_wait(tree->up_cond, tree->up_lock);
        }
        tree->in_update = true;
        single_free_root(tree->root, force);
        tree->in_update = false;
        pthread_mutex_unlock(tree->up_lock);
    }catch(...){
        tree->in_update = false;
        pthread_mutex_unlock(tree->up_lock);
        throw("single_free_root fail");
    }
    free(tree->up_lock);
    free(tree->up_cond);
    free(tree->root);
    free(tree);
    tree = NULL;
}
void single_free_root(Next * p, bool force=false){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
       pn = (Nodes *)p->next;
        if(pn->next_map != NULL){
            for(map<uint32_t, pair<Next *, int> >::iterator it=pn->next_map->begin(); it != pn->next_map->end(); it++){
                single_free_root(it->second.first, force);
                free(it->second.first);
                it->second.first = NULL;
            }
        }
        if(pn->next_map != NULL)pn->next_map->clear();
        if(pn->next_top_wei != NULL) pn->next_top_wei->clear();
        delete pn->next_map;
        delete pn->next_top_wei;
        pn->next_map = NULL;
        pn->next_top_wei = NULL;
        if(pn->oinfo != NULL){
            update_node_oinfo(p, NULL, force);
        }
        free(pn);
        return;
    }
    if(p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->next != NULL){
            single_free_root(p1->next, force);
            free(p1->next);
            p1->next = NULL;
        }
        if(p1->oinfo != NULL){
            update_node_oinfo(p, NULL, force);
        }
        free(p1);
        return;
    }
    if(p->type == 3){
        p3 = (Node_3 *)p->next;
        for(int i = 0; i < 3; i++){
            if(p3->nexts[i] != NULL){
                single_free_root(p3->nexts[i], force);
                free(p3->nexts[i]);
                p3->nexts[i] = NULL;
            }
        }
        if(p3->oinfo != NULL){
            update_node_oinfo(p, NULL, force);
        }
        free(p3);
        return;
    }
    if(p->type == 6){
        p6 = (Node_6 *)p->next;
        for(int i = 0; i < 6; i++){
            if(p6->nexts[i] != NULL){
                single_free_root(p6->nexts[i], force);
                free(p6->nexts[i]);
                p6->nexts[i] = NULL;
            }
        }
        if(p6->oinfo != NULL){
            update_node_oinfo(p, NULL, force);
        }
        free(p6);
        return;
    }
    throw("free tree, p->type not in (0, 1, 3, 6)");
}

Tree * get_tree(){
    Tree * tree = (Tree *)malloc(sizeof(Tree));

    tree->up_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    tree->up_lock2 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    tree->up_cond = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

    pthread_mutex_init(tree->up_lock, NULL);
    pthread_cond_init(tree->up_cond, NULL);

    tree->root = (Next *)malloc(sizeof(Next));
    tree->root->next = get_nodes();
    tree->root->type = 0;

    tree->query_count = 0;
    tree->in_update = false;
    return tree;
}

void up_tree_before_query(Tree * tree){
    pthread_mutex_lock(tree->up_lock);
    while(tree->in_update){
        pthread_cond_wait(tree->up_cond, tree->up_lock);
    }
    tree->query_count += 1;
    pthread_mutex_unlock(tree->up_lock);
}

void up_tree_before_leave(Tree * tree){
    pthread_mutex_lock(tree->up_lock);
    tree->query_count -= 1;
    if(tree->query_count == 0){
        pthread_mutex_unlock(tree->up_lock);
        pthread_cond_broadcast(tree->up_cond);
    }else{
        pthread_mutex_unlock(tree->up_lock);
    }
}

void get_content_prefix_phrases(Tree * tree, const char* content, vector<ResInfo *>& res_list){
    up_tree_before_query(tree);
    try{
        vector<uint32_t> candidate_words; candidate_words.clear();
        if(Utf8ToUnicode32(content, candidate_words) && candidate_words.size() != 0){
            cut_from(tree, candidate_words.begin(), candidate_words.end(), res_list, 0);
        }
        up_tree_before_leave(tree);
    }catch(...){
        up_tree_before_leave(tree);
        throw("get_content_prefix_phrases error, content=" + string(content) + "\n");
    }
}
void get_content_suffix_phrases(Tree * tree,
                                const char* content,
                                vector<ResInfo *> &res_list,
                                int c_limit=(1<<30),
                                int d_limit=(1<<30)){
    up_tree_before_query(tree);
    try{
        vector<uint32_t> candidate_words; candidate_words.clear();
        if(Utf8ToUnicode32(content, candidate_words)){
            Next * endp = get_phrase_endp(tree, candidate_words.begin(), candidate_words.end());
            if(endp != NULL) get_tree_suffix_phrases(endp, content, res_list, c_limit, d_limit);
        }
        up_tree_before_leave(tree);
    }catch(...){
        up_tree_before_leave(tree);
        throw("get_content_suffix_phrases error, content=" + string(content) + "\n");
    }
}
int get_content_suffix_count(Tree * tree, const char* content, vector<ResInfo *> &res_list){
    up_tree_before_query(tree);
    try{
        int res_count = 0;
        vector<uint32_t> candidate_words; candidate_words.clear();
        if(Utf8ToUnicode32(content, candidate_words)){
            Next * endp = get_phrase_endp(tree, candidate_words.begin(), candidate_words.end());
            if(endp != NULL) res_count = get_node_in(endp);
        }
        up_tree_before_leave(tree);
        return res_count;
    }catch(...){
        up_tree_before_leave(tree);
        throw("get_content_suffix_count error, content=" + string(content) + "\n");
    }
}
void get_lcp_suffix_infos(Tree * tree,
                          const char* content,
                          string &lcp_str,
                          vector<ResInfo *> &res_list,
                          int c_limit=(1<<30),
                          int d_limit=(1<<30)
                          ){
    up_tree_before_query(tree);
    try{
        vector<uint32_t> candidate_words; candidate_words.clear();
        if(Utf8ToUnicode32(content, candidate_words)){
            Next * endp = get_lcp_endp(tree, lcp_str, candidate_words.begin(), candidate_words.end());
            if(endp != NULL) get_tree_suffix_phrases(endp, lcp_str, res_list, c_limit, d_limit);
        }
        up_tree_before_leave(tree);
    }catch(...){
        up_tree_before_leave(tree);
        throw("get_lcp_suffix_infos error, content=" + string(content) + "\n");
    }
}
int get_lcp_suffix_count(Tree* tree, const char* content, string &lcp_str){
    up_tree_before_query(tree);
    try{
        int res_count = 0;
        vector<uint32_t> candidate_words; candidate_words.clear();
        if(Utf8ToUnicode32(content, candidate_words)){
            Next * endp = get_lcp_endp(tree, lcp_str, candidate_words.begin(), candidate_words.end());
            if(endp != NULL) res_count= get_node_in(endp);
        }
        up_tree_before_leave(tree);
        return res_count;
    }catch(...){
        up_tree_before_leave(tree);
        throw("get_lcp_suffix_count error, content=" + string(content) + "\n");
    }
}

void * update_thread(void *args){
    Tree * tree = (Tree *)args;
    OInfo *pp[10]; int x = 10;
    for(int i = 0; i < 10; i++){
        pp[i] = (OInfo *)malloc(sizeof(OInfo));
        pp[i]->p = get_node_1();
        pp[i]->type = 3;
    }
    while(true){
        cout<<insert(tree, "121", pp[0], 1)<<endl;
        sleep(5);
        cout<<insert(tree, "122", pp[1], 2)<<endl;
        sleep(5);
        cout<<insert(tree, "123", pp[2], 3)<<endl;
        sleep(5);
        cout<<insert(tree, "124", pp[3], 4)<<endl;
        sleep(5);
        cout<<insert(tree, "124", pp[4], 5)<<endl;
        sleep(5);
        cout<<insert(tree, "125", pp[5], 6)<<endl;
        sleep(5);
        cout<<insert(tree, "126", pp[6], 7)<<endl;
        sleep(5);
        cout<<insert(tree, "127", pp[7], 8)<<endl;
        sleep(5);
        cout<<insert(tree, "128", pp[8], 9)<<endl;
        sleep(5);
        cout<<insert(tree, "129", pp[9], 10)<<endl;
        break;
    }
}
void * query_thread(void *args){
    Tree * tree = (Tree *)args;
    while(true){
        vector<ResInfo *> res_list; res_list.clear();
        get_content_suffix_phrases(tree, "1", res_list, 6);
        pthread_mutex_lock(tree->up_lock2);
        for(vector<ResInfo *>::iterator it = res_list.begin(); it != res_list.end(); it++){
            cout<<(*it)->phrase<<" "<<(*it)->weight<<endl;
            delete (*it);
        }
        cout<<tree->query_count<<" "<<get_micron_second()<<" "<<time(NULL) <<endl;
        sleep(0.1);
        pthread_mutex_unlock(tree->up_lock2);
    }
}

template<class T, int son_count>
struct node{
    T oinfo;
    int sons[son_count];
    vector<int> tt;
    node(){
        cout<<"dd"<<endl;
        tt.push_back(1);
    }
    bool insert();
};
template<class T, int son_count>
bool node<T, son_count>::insert(){
    return true;
}

// g++ -fPIC -lpthread suggest2.cpp -o suggest2.o
int main(){
    /*
    int y = 100;
    node<void *, 5>g;
    node<void *, 5> *k = new node<void *, 5>;
    k->oinfo = &y;
    k->sons[0] = 10;
    cout<<k->insert()<<endl;
    cout<<k->tt[0]<<endl;
    cout<<k->sons[0]<<endl;
    cout<<*((int *)k->oinfo)<<endl;
    return 0;
    */
    Tree * tree = get_tree();

    void *status;
    pthread_t threads[7];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    const int ret0 = pthread_create(&threads[0], 0, update_thread, (void *)tree);
    const int ret1 = pthread_create(&threads[1], 0, update_thread, (void *)tree);
    const int ret2 = pthread_create(&threads[2], 0, update_thread, (void *)tree);
    const int ret3 = pthread_create(&threads[3], 0, query_thread, (void *)tree);
    const int ret4 = pthread_create(&threads[4], 0, query_thread, (void *)tree);
    const int ret5 = pthread_create(&threads[5], 0, query_thread, (void *)tree);
    const int ret6 = pthread_create(&threads[6], 0, query_thread, (void *)tree);

    pthread_attr_destroy(&attr);
    for(int i=0; i < 7; i++ ){
        int rec = pthread_join(threads[i], &status);
        if(rec){
             cout << "Error:unable to join," << rec << endl;
             exit(-1);
        }
        cout << "Main: completed thread id :" << i << "  exiting with status :" << status << endl;
    }
    return 0;

 
    while(true){
        Tree * tree = get_tree();
        OInfo *pp[10]; int x = 10;
        for(int i = 0; i < 10; i++){
            pp[i] = (OInfo *)malloc(sizeof(OInfo));
            pp[i]->p = get_node_1();
            pp[i]->type = 3;
        }
        cout<<insert(tree, "121", pp[0])<<endl;
        cout<<insert(tree, "122", pp[1])<<endl;
        cout<<insert(tree, "123", pp[2])<<endl;
        cout<<insert(tree, "124", pp[3])<<endl;
        cout<<insert(tree, "124", pp[4])<<endl;
        cout<<insert(tree, "125", pp[5])<<endl;
        cout<<insert(tree, "126", pp[6])<<endl;
        cout<<insert(tree, "127", pp[7])<<endl;
        cout<<insert(tree, "128", pp[8])<<endl;
        cout<<insert(tree, "129", pp[9])<<endl;

        OInfo *tp[10];
        for(int i = 0; i < 10; i++){
            tp[i] = (OInfo *)malloc(sizeof(OInfo));
            tp[i]->p = (Node_1 *)malloc(sizeof(Node_1));
            tp[i]->type = 1;
        }
        cout<<insert(tree, "421", tp[0])<<endl;
        cout<<insert(tree, "422", tp[1])<<endl;
        cout<<insert(tree, "423", tp[2])<<endl;
        cout<<insert(tree, "424", tp[3])<<endl;
        cout<<insert(tree, "424", tp[4])<<endl;
        cout<<insert(tree, "425", tp[5])<<endl;
        cout<<insert(tree, "426", tp[6])<<endl;
        cout<<insert(tree, "427", tp[7])<<endl;
        cout<<insert(tree, "428", tp[8])<<endl;
        cout<<insert(tree, "429", tp[9])<<endl;

        for(int i = 0; i < 100000; i++){
            OInfo *oinfo = (OInfo *)malloc(sizeof(OInfo));
            oinfo->p = get_random_str((rand() % 100) + 1);
            oinfo->type = 1;
            char *p = get_random_str((rand() % 100) + 1);
            insert(tree, p, oinfo);
            free(p);
        }
        for(int i = 0; i < 100000; i++){
            OInfo *oinfo = (OInfo *)malloc(sizeof(OInfo));
            oinfo->p = &x;
            oinfo->type = 0;
            char *p = get_random_str((rand() % 100) + 1);
            insert(tree, p, oinfo);
            free(p);
        }
        cout<<insert(tree, "19", NULL)<<endl;
        cout<<insert(tree, "10", NULL)<<endl;
        cout<<insert(tree, "223", NULL)<<endl;
        cout<<insert(tree, "323", NULL)<<endl;
        cout<<insert(tree, "423", NULL)<<endl;
        cout<<insert(tree, "523", NULL)<<endl;
        cout<<insert(tree, "623", NULL)<<endl;
        cout<<insert(tree, "723", NULL)<<endl;
        cout<<insert(tree, "823", NULL)<<endl;
        cout<<insert(tree, "82", NULL)<<endl;
        cout<<insert(tree, "81", NULL)<<endl;
        cout<<insert(tree, "8", NULL)<<endl;

        cout<<"--------"<<endl;
        cout<<remove(tree, "121")<<endl;
        cout<<remove(tree, "124")<<endl;
        cout<<remove(tree, "523")<<endl;
        cout<<remove(tree, "129")<<endl;
        cout<<remove(tree, "8")<<endl;
        cout<<remove(tree, "0")<<endl;
        cout<<"--------"<<endl;

        vector<ResInfo *> res_list; res_list.clear();
        free_tree(tree, true);
        tree = NULL;
        for(int i = 0; i < 10; i++){
            free(pp[i]->p);
            free(pp[i]);
        }
    }
    return 0;
}
