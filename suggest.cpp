# include"Python.h"
# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<map>
# include<vector>
# include<stdint.h>
# include<queue>
# include<algorithm>
# include<queue>
# include<utility>

using namespace std;


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
  string res = "";
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
  cout<<res<<endl;
}


struct OInfo{
    void *p;
    int8_t type;
};


struct Next{
    void *next;
    uint8_t type;
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
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
        throw("p->type is not in (0, 1, 3, 6)");
    }
}
void update_node_oinfo(Next *p, OInfo * oinfo){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    if(p->type == 0){
        pn = (Nodes *)p->next;
        if(pn->oinfo != NULL && pn->oinfo->p != NULL){
            if(pn->oinfo->type == 0){
                PyObject *tmp = (PyObject *)pn->oinfo->p;
                Py_DECREF(tmp);
                pn->oinfo->p = NULL;
            }
            else if(pn->oinfo->type != -1){
                free(pn->oinfo->p);
                pn->oinfo->p = NULL;
            }
        }
        free(pn->oinfo);
        pn->oinfo = oinfo;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->oinfo != NULL && p1->oinfo->p != NULL){
            if(p1->oinfo->type == 0){
                PyObject *tmp = (PyObject *)p1->oinfo->p;
                Py_DECREF(tmp);
                p1->oinfo->p = NULL;
            }
            else if(p1->oinfo->type != -1){
                free(p1->oinfo->p);
                p1->oinfo->p = NULL;
            }
        }
        free(p1->oinfo);
        p1->oinfo = oinfo;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        if(p3->oinfo != NULL && p3->oinfo->p != NULL){
            if(p3->oinfo->type == 0){
                PyObject *tmp = (PyObject *)p3->oinfo->p;
                Py_DECREF(tmp);
                p3->oinfo->p = NULL;
            }
            else if(p3->oinfo->type != -1){
                free(p3->oinfo->p);
                p3->oinfo->p = NULL;
            }
        }
        free(p3->oinfo);
        p3->oinfo = oinfo;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        if(p6->oinfo != NULL && p6->oinfo->p != NULL){
            if(p6->oinfo->type == 0){
                PyObject *tmp = (PyObject *)p6->oinfo->p;
                Py_DECREF(tmp);
                p6->oinfo->p = NULL;
            }
            else if(p6->oinfo->type != -1){
                free(p6->oinfo->p);
                p6->oinfo->p = NULL;
            }
        }
        free(p6->oinfo);
        p6->oinfo = oinfo;
    }else{
        throw("p->type is not in (0, 1, 3, 6)");
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


bool insert(Next* root, const char* content, OInfo *oinfo, int weight=0){
    int dep = 0; Next* p = root; Next** parent = &root;

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


bool remove(Next* root, const char* content){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    map<uint32_t, pair<Next *, int> >::iterator ik; Next* p = root;
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
            throw("p->type not in (0, 1, 3, 6)");
        }
    }

    bool res = get_node_end(p);
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
    update_node_oinfo(p, NULL);
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

int32_t cut_from(Next* root,
                 vector<uint32_t>:: iterator start,
                 vector<uint32_t>:: iterator end,
                 vector<ResInfo *> &res_list,
                 int32_t offset_position=0){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
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
                             const string &res_str,
                             vector<ResInfo *> &res_list,
                             int c_limit=(1<<30),
                             int d_limit=(1<<30)){
    priority_queue<pair<pair<Next *, int>, pair<string, int> >, vector<pair<pair<Next *, int>, pair<string, int> > >, cmp2>queue;
    while(!queue.empty()) queue.pop();
    priority_queue<pair<pair<Next *, int>, pair<string, int> >, vector<pair<pair<Next *, int>, pair<string, int> > >, cmp2>result;
    while(!queue.empty()) queue.pop();
    queue.push(make_pair(make_pair(p, get_node_weight(p)), make_pair(string(res_str), 0)));

    vector<pair<pair<Next *, int>, pair<string, int> > > all_nodes;
    while(queue.size()){
        all_nodes.clear();
        while(queue.size()){
            all_nodes.push_back(queue.top()); queue.pop();
        }
        reverse(all_nodes.begin(), all_nodes.end());
        for(vector<pair<pair<Next *, int>, pair<string, int> > >::iterator it = all_nodes.begin(); it != all_nodes.end(); it++){
            Next *p = it->first.first;
            string res_str = it->second.first;
            int dep = it->second.second;
            if(dep > d_limit) continue;

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
                    if(queue.size() >= c_limit && it->second <= queue.top().first.second) break;
                    if(result.size() >= c_limit && it->second <= result.top().first.second) break;
                    string tmp_str = res_str; Unicode32ToUtf8(it->first, tmp_str);
                    //cout<<"1"<<" "<<it->second<<" "<<(*pn->next_map).size()<<" "; PrintUnicode(it->first);
                    queue.push(make_pair(make_pair((*pn->next_map)[it->first].first, it->second), make_pair(tmp_str, dep + 1)));
                    while(queue.size() > c_limit) queue.pop();
                    //cout<<"2"<<endl;
                    if((++j) > c_limit)throw("error sort");
                    //cout<<"d"<<endl;
                }
            }else if (p->type == 1){
                Node_1* p1 = (Node_1 *)p->next;
                if(p1->next == NULL) continue;
                if(queue.size() >= c_limit && p1->max_wei <= queue.top().first.second) continue;
                if(result.size() >= c_limit && p1->max_wei <= result.top().first.second) continue;
                string tmp_str = res_str; Unicode32ToUtf8(p1->word, tmp_str);
                queue.push(make_pair(make_pair(p1->next, p1->max_wei), make_pair(tmp_str, dep + 1)));
                while(queue.size() > c_limit) queue.pop();
            }else if (p->type == 3){
                Node_3* p3 = (Node_3 *)p->next;
                for(uint32_t i = 0; i < 3; i++){
                    if(p3->nexts[i] == NULL) continue;
                    if(queue.size() >= c_limit && p3->max_wei[i] <= queue.top().first.second) continue;
                    if(result.size() >= c_limit && p3->max_wei[i] <= result.top().first.second) continue;
                    string tmp_str = res_str; Unicode32ToUtf8(p3->words[i], tmp_str);
                    queue.push(make_pair(make_pair(p3->nexts[i], p3->max_wei[i]), make_pair(tmp_str, dep + 1)));
                    while(queue.size() > c_limit) queue.pop();
                }
            } else if (p->type == 6){
                Node_6* p6 = (Node_6 *)p->next;
                for(uint32_t i = 0; i < 6; i++){
                    if(p6->nexts[i] == NULL) continue;
                    if(queue.size() >= c_limit && p6->max_wei[i] <= queue.top().first.second) continue;
                    if(result.size() >= c_limit && p6->max_wei[i] <= result.top().first.second) continue;
                    string tmp_str = res_str; Unicode32ToUtf8(p6->words[i], tmp_str);
                    queue.push(make_pair(make_pair(p6->nexts[i], p6->max_wei[i]), make_pair(tmp_str, dep + 1)));
                    while(queue.size() > c_limit) queue.pop();
                }
            } else{
                throw("p->type not in (0, 1, 3, 6)");
            }
            while(queue.size() > c_limit) queue.pop();
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


Next * get_phrase_endp(Next* root,
                       vector<uint32_t>:: iterator start,
                       vector<uint32_t>:: iterator end
                       ){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
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

Next * get_lcp_endp(Next* root,
                   string &res_str,
                   vector<uint32_t>:: iterator start,
                   vector<uint32_t>:: iterator end
                   ){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
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


void free_tree(Next* p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    //printf("%d, kk-----\n", p->type);
    if(p->type == 0){
       pn = (Nodes *)p->next;
        if(pn->next_map != NULL){
            for(map<uint32_t, pair<Next *, int> >::iterator it=pn->next_map->begin(); it != pn->next_map->end(); it++){
                //string str = ""; uint32_t tmp=it->first; Unicode32ToUtf8(tmp, str);
                //printf("%d, %s---%d-\n", p->type, str.c_str(), pn->next_map->size());
                free_tree(it->second.first);
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
        if(pn->oinfo != NULL && pn->oinfo->p != NULL){
            if(pn->oinfo->type == 0){
                PyObject * tmp = (PyObject *)pn->oinfo->p;
                Py_DECREF(tmp);
            }else{
                free(pn->oinfo->p);
                pn->oinfo->p = NULL;
            }
        }
        free(pn->oinfo);
        pn->oinfo = NULL;
        free(pn);
        return;
    }
    if(p->type == 1){
        p1 = (Node_1 *)p->next;
        if(p1->next != NULL){
            //string str = ""; Unicode32ToUtf8(p1->word, str);
            //printf("%d, %s-----\n", p->type, str.c_str());
            free_tree(p1->next);
            free(p1->next);
            p1->next = NULL;
        }
        if(p1->oinfo != NULL && p1->oinfo->p != NULL){
            if(p1->oinfo->type == 0){
                PyObject * tmp = (PyObject *)p1->oinfo->p;
                Py_DECREF(tmp);
            }else{
                free(p1->oinfo->p);
                p1->oinfo->p = NULL;
            }
        }
        free(p1->oinfo);
        p1->oinfo = NULL;
        free(p1);
        return;
    }
    if(p->type == 3){
        p3 = (Node_3 *)p->next;
        for(int i = 0; i < 3; i++){
            if(p3->nexts[i] != NULL){
                //string str = ""; Unicode32ToUtf8(p3->words[i], str);
                //printf("%d, %s-----\n", p->type, str.c_str());
                free_tree(p3->nexts[i]);
                free(p3->nexts[i]);
                p3->nexts[i] = NULL;
            }
        }
        if(p3->oinfo != NULL && p3->oinfo->p != NULL){
            if(p3->oinfo->type == 0){
                PyObject * tmp = (PyObject *)p3->oinfo->p;
                Py_DECREF(tmp);
            }else{
                free(p3->oinfo->p);
                p3->oinfo->p = NULL;
            }
        }
        free(p3->oinfo);
        p3->oinfo = NULL;
        free(p3);
        return;
    }
    if(p->type == 6){
        p6 = (Node_6 *)p->next;
        for(int i = 0; i < 6; i++){
            if(p6->nexts[i] != NULL){
                //string str = ""; Unicode32ToUtf8(p6->words[i], str);
                //printf("%d, %s-----\n", p->type, str.c_str());
                free_tree(p6->nexts[i]);
                free(p6->nexts[i]);
                p6->nexts[i] = NULL;
            }
        }
        if(p6->oinfo != NULL && p6->oinfo->p != NULL){
            if(p6->oinfo->type == 0){
                PyObject * tmp = (PyObject *)p6->oinfo->p;
                Py_DECREF(tmp);
            }else{
                free(p6->oinfo->p);
                p6->oinfo->p = NULL;
            }
        }
        free(p6->oinfo);
        p6->oinfo = NULL;
        free(p6);
        return;
    }
    throw("free tree, p->type not in (0, 1, 3, 6)");
}


Next * get_root(){
    Next *root = (Next *)malloc(sizeof(Next));
    Node_1 * tmp_node = get_node_1();
    root->next = tmp_node;
    root->type = 1;
    return root;
}
void get_content_prefix_phrases(Next* root, const char* content, vector<ResInfo *>& res_list){
    vector<uint32_t> candidate_words; candidate_words.clear();
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    if(candidate_words.size() == 0) return;
    cut_from(root, candidate_words.begin(), candidate_words.end(), res_list, 0);
}
void get_content_suffix_phrases(Next* root, const char* content, vector<ResInfo *> &res_list, int c_limit=(1<<30), int d_limit=(1<<30)){
    vector<uint32_t> candidate_words; candidate_words.clear();
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    //if(candidate_words.size() == 0) return;
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return;
    get_tree_suffix_phrases(endp, content, res_list, c_limit, d_limit);
}
int get_content_suffix_count(Next* root, const char* content, vector<ResInfo *> &res_list){
    vector<uint32_t> candidate_words; candidate_words.clear();
    if(!Utf8ToUnicode32(content, candidate_words)) return 0;
    //if(candidate_words.size() == 0) return 0;
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return 0;
    return get_node_in(endp);
}
void get_lcp_suffix_infos(Next* root, const char* content, vector<ResInfo *> &res_list, int c_limit=(1<<30), int d_limit=(1<<30)){
    vector<uint32_t> candidate_words; candidate_words.clear();
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    //if(candidate_words.size() == 0) return;
    string res_str = "";
    Next * endp = get_lcp_endp(root, res_str, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return;
    get_tree_suffix_phrases(endp, res_str, res_list, c_limit, d_limit);
}
int get_lcp_suffix_count(Next* root, const char* content, vector<ResInfo *> &res_list){
    vector<uint32_t> candidate_words; candidate_words.clear();
    if(!Utf8ToUnicode32(content, candidate_words)) return 0;
    //if(candidate_words.size() == 0) return 0;
    string res_str = "";
    Next * endp = get_lcp_endp(root, res_str, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return 0;
    return get_node_in(endp);
}


extern "C" void * get_root_prx(){
    void *p = get_root();
    return p;
}
extern "C" void free_root_prx(void* p){
    PyGILState_STATE gstate = PyGILState_Ensure();
    Next* root = (Next *)p;
    free_tree(root);
    free(root);
    PyGILState_Release(gstate);
}
extern "C" bool insert_prx(void* p, const char* content, PyObject * oinfo, int weight){
    Py_INCREF(oinfo);
    Next* root = (Next *)p;
    OInfo* tmp_info = get_oinfo(oinfo, 0);
    return insert(root, content, tmp_info, weight);
}
extern "C" bool remove_prx(void* p, const char *content){
    Next* root = (Next *)p;
    return remove(root, content);
}
extern "C" PyObject * get_phrase_prefix_infos_prx(void *p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_content_prefix_phrases(root, content, res_list);

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *oplist = PyList_New(res_list.size());
    for(uint32_t j = 0; j < res_list.size(); j++){
        PyObject* pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", res_list[j]->phrase.c_str()));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", res_list[j]->position));
        if(res_list[j]->oinfo->type == 0){
            Py_INCREF((PyObject *)res_list[j]->oinfo->p);
            PyTuple_SetItem(pTuple, 2, Py_BuildValue("N", (PyObject *)res_list[j]->oinfo->p));
        }else if(res_list[j]->oinfo->type == -1){
            Py_INCREF(Py_None);
            PyTuple_SetItem(pTuple, 2, Py_None);
        }
        PyList_SetItem(oplist, j, pTuple);
        //printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        delete res_list[j];
    }
    PyGILState_Release(gstate);
    return oplist;
}
extern "C" PyObject * get_phrase_suffix_infos_prx(void *p, const char *content, int c_limit=(1<<30), int d_limit=(1<<30)){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_content_suffix_phrases(root, content, res_list, c_limit, d_limit);

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *oplist = PyList_New(res_list.size());
    for(uint32_t j = 0; j < res_list.size(); j++){
        PyObject* pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", res_list[j]->phrase.c_str()));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", res_list[j]->weight));
        if(res_list[j]->oinfo->type == 0){
            Py_INCREF((PyObject *)res_list[j]->oinfo->p);
            PyTuple_SetItem(pTuple, 2, Py_BuildValue("N", (PyObject *)res_list[j]->oinfo->p));
        }else if(res_list[j]->oinfo->type == -1){
            Py_INCREF(Py_None);
            PyTuple_SetItem(pTuple, 2, Py_None);
        }
        PyList_SetItem(oplist, j, pTuple);
        //printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        delete res_list[j];
    }
    res_list.clear();
    PyGILState_Release(gstate);
    return oplist;
}
extern "C" int get_phrase_suffix_count_prx(void *p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    return get_content_suffix_count(root, content, res_list);
}
extern "C" PyObject * get_lcp_suffix_infos_prx(void *p, const char *content, int c_limit=(1<<30), int d_limit=(1<<30)){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_lcp_suffix_infos(root, content, res_list, c_limit, d_limit);

    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *oplist = PyList_New(res_list.size());
    for(uint32_t j = 0; j < res_list.size(); j++){
        PyObject* pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", res_list[j]->phrase.c_str()));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", res_list[j]->weight));
        if(res_list[j]->oinfo->type == 0){
            Py_INCREF((PyObject *)res_list[j]->oinfo->p);
            PyTuple_SetItem(pTuple, 2, Py_BuildValue("N", (PyObject *)res_list[j]->oinfo->p));
        }else if(res_list[j]->oinfo->type == -1){
            Py_INCREF(Py_None);
            PyTuple_SetItem(pTuple, 2, Py_None);
        }
        PyList_SetItem(oplist, j, pTuple);
        //printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        delete res_list[j];
    }
    res_list.clear();
    PyGILState_Release(gstate);
    return oplist;
}
extern "C" int get_lcp_suffix_count_prx(void *p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    return get_lcp_suffix_count(root, content, res_list);
}
extern "C" bool exist_phrase_prx(void *p, const char* content){
    Next* root = (Next *)p;
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;
    if(candidate_words.size() == 0) return false;
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return false;
    return get_node_end(endp);
}
extern "C" PyObject * get_kv_prx(void *p, const char* content){
    Next* root = (Next *)p;
    vector<uint32_t> candidate_words;

    PyGILState_STATE gstate = PyGILState_Ensure();
    if(!Utf8ToUnicode32(content, candidate_words)){
        Py_INCREF(Py_None);
        PyGILState_Release(gstate);
        return Py_None;
    }
    if(candidate_words.size() == 0){
        Py_INCREF(Py_None);
        PyGILState_Release(gstate);
        return Py_None;
    }
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL){
        Py_INCREF(Py_None);
        PyGILState_Release(gstate);
        return Py_None;
    }
    const OInfo *oinfo = get_node_oinfo(endp);
    if(oinfo == NULL){
        Py_INCREF(Py_None);
        PyGILState_Release(gstate);
        return Py_None;
    }
    else if(oinfo->type == 0){
        Py_INCREF((PyObject *)oinfo->p);
        PyGILState_Release(gstate);
        return (PyObject *)oinfo->p;
    }else{
        PyGILState_Release(gstate);
        throw("oinfo->type not python obj");
    }
}
extern "C" PyObject * test(){
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject *oplist = PyTuple_New(500);
    for(int32_t j = 0; j < 500; j++){
        PyObject * pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", "b"));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", 1));
        PyTuple_SetItem(pTuple, 2, Py_BuildValue("s", "a"));
        PyTuple_SetItem(oplist, j, pTuple);
    }
    PyGILState_Release(gstate);
    return oplist;
}


/*
PyObject* wrap_insert(PyObject* self, PyObject* args){
    void *p;
    Next *root;
    char *content;
    if (!PyArg_ParseTuple(args, "sp", &p, &content)) return NULL;
    root = (Next *)p;
    bool res = insert(root, content);
    return Py_BuildValue("b", res);
}


PyObject* wrap_remove(PyObject* self, PyObject* args){
    char *str1, *str2; int dis;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    string path = get_edit_path(str1, str2, dis);
    return Py_BuildValue("is", dis, path.c_str());
}

PyObject* wrap_free(PyObject* self, PyObject* args){
    char *str1, *str2; int dis;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    string path = get_edit_path(str1, str2, dis);
    return Py_BuildValue("is", dis, path.c_str());
}


PyObject* wrap_get_root(PyObject* self, PyObject* args){
    char *str1, *str2; int dis;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    string path = get_edit_path(str1, str2, dis);
    return Py_BuildValue("is", dis, path.c_str());
}


static PyMethodDef treeMethods[] = {
        {"insert", wrap_insert, METH_VARARGS, "doc: wrap_remove"},
        {"remove", wrap_remove, METH_VARARGS, "doc: wrap_remove"},
        {"free", wrap_free, METH_VARARGS, "doc: wrap_free"},
        {"get_root", wrap_get_root, METH_VARARGS, "doc: get_root"},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initLD(void) {
        (void) Py_InitModule("LD", treeMethods);
}
*/

// g++ -fPIC suggest.cpp -I/usr/local/app/service/virtualenvs/NLP/include/python2.7 -shared -o suggest_lib.so
int main(){
    while(true){
        Next *root = get_root(); OInfo *pp[10]; void *tt[10];
        for(int i = 0; i < 10; i++){
            pp[i] = (OInfo *)malloc(sizeof(OInfo));
            tt[i] = get_node_1();
            pp[i]->p = tt[i];
            pp[i]->type = 1;
        }
        cout<<insert(root, "121", pp[0])<<endl;
        cout<<insert(root, "122", pp[1])<<endl;
        cout<<insert(root, "123", pp[2])<<endl;
        cout<<insert(root, "124", pp[3])<<endl;
        cout<<insert(root, "124", pp[4])<<endl;
        cout<<insert(root, "125", pp[5])<<endl;
        cout<<insert(root, "126", pp[6])<<endl;
        cout<<insert(root, "127", pp[7])<<endl;
        cout<<insert(root, "128", pp[8])<<endl;
        cout<<insert(root, "129", pp[9])<<endl;

        cout<<insert(root, "133", NULL)<<endl;
        cout<<insert(root, "143", NULL)<<endl;
        cout<<insert(root, "153", NULL)<<endl;
        cout<<insert(root, "163", NULL)<<endl;
        cout<<insert(root, "173", NULL)<<endl;
        cout<<insert(root, "183", NULL)<<endl;

        cout<<insert(root, "19", NULL)<<endl;
        cout<<insert(root, "10", NULL)<<endl;
        cout<<insert(root, "223", NULL)<<endl;
        cout<<insert(root, "323", NULL)<<endl;
        cout<<insert(root, "423", NULL)<<endl;
        cout<<insert(root, "523", NULL)<<endl;
        cout<<insert(root, "623", NULL)<<endl;
        cout<<insert(root, "723", NULL)<<endl;
        cout<<insert(root, "823", NULL)<<endl;
        cout<<insert(root, "82", NULL)<<endl;
        cout<<insert(root, "81", NULL)<<endl;
        cout<<insert(root, "8", NULL)<<endl;

        vector<ResInfo *> res_list; res_list.clear();
        cout<<remove(root, "121")<<endl;
        cout<<remove(root, "124")<<endl;
        cout<<remove(root, "523")<<endl;
        cout<<remove(root, "129")<<endl;
        cout<<remove(root, "8")<<endl;
        cout<<remove(root, "0")<<endl;

        res_list.clear();
        cout<<"insert end"<<endl;
        free_tree(root); free(root); root = NULL;
        for(int i = 0; i < 10; i++){
            //void * tt = const_cast<void *>(pp[i]->p);
            free(tt[i]);
        }
        cout<<"end"<<endl;
    }
    return 0;
}
