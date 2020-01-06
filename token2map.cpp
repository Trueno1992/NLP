# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<map>
# include<vector>
# include<stdint.h>
# include"Python.h"
using namespace std;

/*
PyObject* PyList_New(int size)
{
    PyListObject *op;
    size_t nbytes;


    nbytes = size * sizeof(PyObject *);
    // Check for overflow 
    if (nbytes / sizeof(PyObject *) != (size_t)size)
        return PyErr_NoMemory();

    //为PyListObject申请空间
    if (num_free_lists) {
        //使用缓冲池
        num_free_lists--;
        op = free_lists[num_free_lists];
        _Py_NewReference((PyObject *)op);
    } else {
        //缓冲池中没有可用的对象，创建对象
        op = PyObject_GC_New(PyListObject, &PyList_Type);
    }
    //为PyListObject对象中维护的元素列表申请空间
    if (size <= 0)
        op->ob_item = NULL;
    else {
        op->ob_item = (PyObject **) PyMem_MALLOC(nbytes);
        memset(op->ob_item, 0, nbytes);
    }
    op->ob_size = size;
    op->allocated = size;
    _PyObject_GC_TRACK(op);
    return (PyObject *) op;
}
*/

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


struct OInfo{
    void *p;
    int8_t type;
    /*
    OInfo(void *tmp_p, uint8_t tmp_type){
        p = tmp_p;
        type = tmp_type;
    }
    */
};


struct Next{
    void *next;
    uint8_t type;
};


struct Node_1{
    bool is_end;
    uint32_t word;
    Next *next;
    OInfo *oinfo;
};


struct Node_3{
    bool is_end;
    uint32_t words[3];
    Next *nexts[3];
    OInfo *oinfo;
};


struct Node_6{
    bool is_end;
    uint32_t words[6];
    Next *nexts[6];
    OInfo *oinfo;
};


struct Nodes{
    bool is_end;
    map<uint32_t, Next *> *next_map;
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
    tmp_node->next = NULL;
    tmp_node->oinfo = NULL;
    return tmp_node;
}
Node_3 * get_node_3(){
    Node_3 * new_node = (Node_3 *)malloc(sizeof(Node_3));
    new_node->is_end = false;
    for(unsigned int i = 0; i < 3; i ++){
        new_node->words[i] = 0;
        new_node->nexts[i] = NULL;
    }
    new_node->oinfo = NULL;
    return new_node;
}
Node_6 * get_node_6(){
    Node_6 * new_node = (Node_6 *)malloc(sizeof(Node_6));
    new_node->is_end = false;
    for(unsigned int i = 0; i < 6; i ++){
        new_node->words[i] = 0;
        new_node->nexts[i] = NULL;
    }
    new_node->oinfo = NULL;
    return new_node;
}
Nodes * get_nodes(){
    Nodes * new_node = (Nodes *)malloc(sizeof(Nodes));
    new_node->is_end = false;
    new_node->next_map = new map<uint32_t, Next *>;
    new_node->next_map->clear();
    new_node->oinfo = NULL;
    return new_node;
}


bool insert(Next* root, const char* content, OInfo *oinfo){
    int dep = 0; Next* p = root; Next** parent = &root;
    //printf("%d --\n", root->type);

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, Next *>::iterator ik;
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++, dep++){
        //printf("%d, %d\n", p->type, *it);
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik != pn->next_map->end()){
                parent = &ik->second;
                p = ik->second;
            }else{
                Next * tmp_next = (Next *)malloc(sizeof(Next));
                tmp_next->next = get_node_1();
                tmp_next->type = 1;

                (*pn->next_map)[*it] = tmp_next;
                parent = &tmp_next;
                p = tmp_next;
            }
        }else if(p->type == 1){
            p1 = (Node_1 * )p->next;
            if(p1->next != NULL && p1->word == *it){
                parent = &(p1->next);
                p = p1->next;
            }else{
                if(p1->next == NULL){
                    p1->next = (Next *)malloc(sizeof(Next));
                    p1->next->next = get_node_1();
                    p1->next->type = 1;
                    p1->word = *it;

                    parent = &(p1->next);
                    p = p1->next;
                }else{
                    Node_3 * new_node = get_node_3();
                    new_node->words[0] = p1->word;
                    new_node->nexts[0] = p1->next;
                    new_node->is_end = p1->is_end;
                    new_node->oinfo = p1->oinfo;

                    new_node->nexts[1] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[1]->next = get_node_1();
                    new_node->nexts[1]->type = 1;
                    new_node->words[1] = *it;

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
            bool is_find = false;
            for(unsigned int i = 0; i < 3; i++){
                if(p3->nexts[i] == NULL) break;
                if(p3->words[i] == *it){
                    is_find = true;
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
                        new_node->is_end = p3->is_end;
                        new_node->oinfo = p3->oinfo;
                    }
                    new_node->nexts[3] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[3]->next = get_node_1();
                    new_node->nexts[3]->type = 1;
                    new_node->words[3] = *it;

                    (*parent)->next = new_node;
                    (*parent)->type = 6;
                    free(p3);

                    parent = &(new_node->nexts[3]);
                    p = new_node->nexts[3];
                }
            }
        }else if(p->type == 6){
            p6 = (Node_6 * )p->next;
            bool is_find = false;
            for(unsigned int i = 0; i < 6; i++){
                if(p6->nexts[i] == NULL) break;
                if(p6->words[i] == *it){
                    is_find = true;
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
                        (*new_node->next_map)[p6->words[i]] = p6->nexts[i];
                    }
                    (*new_node->next_map)[*it] = tt_next;
                    new_node->is_end = p6->is_end;

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
    update_node_oinfo(p, oinfo);
    update_node_end(p, true);
    return true;
}


bool remove(Next* root, const char* content){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    map<uint32_t, Next *>::iterator ik; Next* p= root;
    for(vector<uint32_t>::iterator it = candidate_words.begin(); it != candidate_words.end(); it++){
        if(p->type == 0){
           pn = (Nodes *)p->next;
            if(pn->next_map != NULL){
                ik = pn->next_map->find(*it);
                if(ik != pn->next_map->end()){
                    p = ik->second;
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
    update_node_oinfo(p, NULL);
    update_node_end(p, false);
    return res;
}


struct ResInfo{
    string phrase;
    uint32_t position;
    uint32_t phrase_len;
    const OInfo *oinfo;
};


/*
return: 从当前位置切分的最长切分位置
*/
int32_t cut_from(Next* root,
                 vector<uint32_t>:: iterator start,
                 vector<uint32_t>:: iterator end,
                 vector<ResInfo *> &res_list,
                 int32_t offset_position=0){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, Next *>::iterator ik;
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
            p = ik->second;
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

/*最大切分*/
int32_t cut_max(Next* root,
                vector<uint32_t>:: iterator start,
                vector<uint32_t>:: iterator end,
                ResInfo * res_info,
                int32_t offset_position=0){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, Next *>::iterator ik;
    string res_str = "";
    int32_t max_position = -1, str_len = 0;
    for(vector<uint32_t>:: iterator it = start; it != end; it++, offset_position++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = ik->second;
            str_len += 1;
            if(get_node_end(p)){
                res_info->phrase = res_str;
                res_info->position = offset_position;
                res_info->phrase_len = str_len;
                res_info->oinfo = get_node_oinfo(p);
                max_position = max(max_position, offset_position);
            }
        }
        else if (p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->word != *it)return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = p1->next;
            str_len += 1;
            if(get_node_end(p)){
                res_info->phrase = res_str;
                res_info->position = offset_position;
                res_info->phrase_len = str_len;
                res_info->oinfo = get_node_oinfo(p);
                max_position = max(max_position, offset_position);
            }
        }
        else if (p->type == 3){
            p3 = (Node_3 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 3; i++){
                if(p3->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p3->nexts[i];
                    str_len += 1;

                    if(get_node_end(p)){
                        res_info->phrase = res_str.c_str();
                        res_info->position = offset_position;
                        res_info->phrase_len = str_len;
                        res_info->oinfo = get_node_oinfo(p);
                        max_position = max(max_position, offset_position);
                    }
                    find = true;
                    break;
                }
            }
            if(!find) return max_position;
        }
        else if (p->type == 6){
            p6 = (Node_6 *)p->next;
            bool find = false;
            for(uint32_t i = 0; i < 6; i++){
                if(p6->words[i] == *it){
                    Unicode32ToUtf8(*it, res_str);
                    p = p6->nexts[i];
                    str_len += 1;
                    if(get_node_end(p)){
                        res_info->phrase = res_str;
                        res_info->position = offset_position;
                        res_info->phrase_len = str_len;
                        res_info->oinfo = get_node_oinfo(p);
                        max_position = max(max_position, offset_position);
                    }
                    find = true;
                    break;
                }
            }
            if(!find) return max_position;
        }
    }
    return max_position;
}


void get_tree_suffix_phrases(Next * p, const string &res_str, vector<ResInfo *> &res_list){
    if (get_node_end(p)){
        ResInfo *res_info = new ResInfo();
        res_info->phrase = res_str;
        res_info->position = -1;
        res_info->oinfo = get_node_oinfo(p);
        res_list.push_back(res_info);
    }
    if (p->type == 0){
        Nodes* pn = (Nodes *)p->next;
        if (pn->next_map == NULL) return;
        for(map<uint32_t, Next *>::iterator it = pn->next_map->begin(); it != pn->next_map->end(); it++){
            if (it->second == NULL) return;
            p = it->second;
            uint32_t word = it->first;
            string tmp_str = res_str;
            Unicode32ToUtf8(it->first, tmp_str);
            get_tree_suffix_phrases(p, tmp_str, res_list);
        }
    }else if (p->type == 1){
        Node_1* p1 = (Node_1 *)p->next;
        if(p1->next == NULL) return;
        p = p1->next;
        string tmp_str = res_str;
        Unicode32ToUtf8(p1->word, tmp_str);
        get_tree_suffix_phrases(p, tmp_str, res_list);
    }else if (p->type == 3){
        Node_3* p3 = (Node_3 *)p->next;
        for(uint32_t i = 0; i < 3; i++){
            if(p3->nexts[i] == NULL) continue;
            p = p3->nexts[i];
            string tmp_str = res_str;
            Unicode32ToUtf8(p3->words[i], tmp_str);
            get_tree_suffix_phrases(p, tmp_str, res_list);
        }
    } else if (p->type == 6){
        Node_6* p6 = (Node_6 *)p->next;
        for(uint32_t i = 0; i < 6; i++){
            if(p6->nexts[i] == NULL) continue;
            p = p6->nexts[i];
            string tmp_str = res_str;
            Unicode32ToUtf8(p6->words[i], tmp_str);
            get_tree_suffix_phrases(p, tmp_str, res_list);
        }
    } else{
        throw("p->type not in (0, 1, 3, 6)");
    }
}


Next * get_phrase_endp(Next* root,
                       vector<uint32_t>:: iterator start,
                       vector<uint32_t>:: iterator end
                       ){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, Next *>::iterator ik;
    for(vector<uint32_t>:: iterator it = start; it != end; it++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return NULL;
            p = ik->second;
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


void free_tree(Next* p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    //printf("%d, kk-----\n", p->type);
    if(p->type == 0){
       pn = (Nodes *)p->next;
        if(pn->next_map != NULL){
            for(map<uint32_t, Next *>::iterator it=pn->next_map->begin(); it != pn->next_map->end(); it++){
                //string str = ""; uint32_t tmp=it->first; Unicode32ToUtf8(tmp, str);
                //printf("%d, %s---%d-\n", p->type, str.c_str(), pn->next_map->size());
                free_tree(it->second);
                free(it->second);
                it->second = NULL;
            }
        }
        pn->next_map->clear();
        delete pn->next_map;
        pn->next_map = NULL;
        if(pn->oinfo != NULL && pn->oinfo->p != NULL){
            if(pn->oinfo->type == 0){
                PyObject * tmp = (PyObject *)pn->oinfo->p;
                Py_DECREF(tmp);
                cout<<"111111"<<endl;
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
                cout<<"2222222"<<endl;
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
                cout<<"333333"<<endl;
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
                cout<<"444444"<<endl;
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
void get_content_all(Next* root, const char* content, vector<ResInfo *>& res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    int32_t position = 0; res_list.clear();
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++, position++){
        cut_from(root, it, candidate_words.end(), res_list, position);
    }
}
OInfo global_oinfo = {NULL, -1};
void cut_content_all(Next* root, const char* content, vector<ResInfo *>& res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    int32_t position = 0, max_position = -1, now_max_position; res_list.clear();
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++, position++){
        now_max_position = cut_from(root, it, candidate_words.end(), res_list, position);
        if(now_max_position == -1 && max_position < position){
            ResInfo *res_info = new ResInfo();
            res_info->phrase = "";
            Unicode32ToUtf8(*it, res_info->phrase);
            res_info->position = position;
            res_info->oinfo = NULL;
            res_list.push_back(res_info);
        }
        max_position = max(max_position, max(position, now_max_position));
    }
}
void cut_content_max(Next* root, const char* content, vector<ResInfo *>& res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    int32_t position = 0, now_max_position; res_list.clear();
    vector<uint32_t>:: iterator it = candidate_words.begin();
    while (it != candidate_words.end()){
        ResInfo * res_info = new ResInfo();
        now_max_position = cut_max(root, it, candidate_words.end(), res_info, position);
        if(now_max_position == -1){
            res_info->phrase = "";
            Unicode32ToUtf8(*it, res_info->phrase);
            res_info->position = position;
            res_info->oinfo = NULL;
        }else{
            it += res_info->phrase_len - 1;
            position = res_info->position; 
        }
        res_list.push_back(res_info);
        it++;
        position++;
    }
}
void get_content_prefix_phrases(Next* root, const char* content, vector<ResInfo *>& res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    if(candidate_words.size() == 0) return;
    cut_from(root, candidate_words.begin(), candidate_words.end(), res_list, 0);
}
void get_content_suffix_phrases(Next* root, const char* content, vector<ResInfo *> &res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    if(candidate_words.size() == 0) return;
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL) return;
    get_tree_suffix_phrases(endp, content, res_list);
}

extern "C" void * get_root_prx(){
    void *p = get_root();
    return p;
}
extern "C" void free_root_prx(void* p){
    Next* root = (Next *)p;
    free_tree(root);
    free(root);
}
extern "C" bool insert_prx(void* p, const char* content, PyObject * oinfo){
    Py_INCREF(oinfo);
    Next* root = (Next *)p;
    OInfo* tmp_info = get_oinfo(oinfo, 0);
    return insert(root, content, tmp_info);
}
extern "C" bool remove_prx(void* p, const char *content){
    Next* root = (Next *)p;
    return remove(root, content);
}
extern "C" PyObject * cutall_prx(void* p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    cut_content_all(root, content, res_list);

    PyObject *oplist = PyList_New(res_list.size());
    for(uint32_t j = 0; j < res_list.size(); j++){
        PyObject* pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", res_list[j]->phrase.c_str()));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", res_list[j]->position));

        if(res_list[j]->oinfo != NULL && res_list[j]->oinfo->type == 0){
            Py_INCREF((PyObject *)res_list[j]->oinfo->p);
            PyTuple_SetItem(pTuple, 2, Py_BuildValue("N", (PyObject *)res_list[j]->oinfo->p));
        }else if(res_list[j]->oinfo == NULL){
            Py_INCREF(Py_None);
            PyTuple_SetItem(pTuple, 2, Py_None);
        }
        PyList_SetItem(oplist, j, pTuple);
        //printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        delete res_list[j];
    }
    return oplist;
}
extern "C" PyObject * cutmax_prx(void* p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    cut_content_max(root, content, res_list);

    PyObject *oplist = PyList_New(res_list.size());
    for(uint32_t j = 0; j < res_list.size(); j++){
        PyObject* pTuple = PyTuple_New(3);
        assert(PyTuple_Check(pTuple));
        assert(PyTuple_Size(pTuple) == 3);
        PyTuple_SetItem(pTuple, 0, Py_BuildValue("s", res_list[j]->phrase.c_str()));
        PyTuple_SetItem(pTuple, 1, Py_BuildValue("i", res_list[j]->position));
        PyList_SetItem(oplist, j, pTuple);
        if(res_list[j]->oinfo != NULL && res_list[j]->oinfo->type == 0){
            Py_INCREF((PyObject *)res_list[j]->oinfo->p);
            PyTuple_SetItem(pTuple, 2, Py_BuildValue("N", (PyObject *)res_list[j]->oinfo->p));
        }else if(res_list[j]->oinfo == NULL){
            Py_INCREF(Py_None);
            PyTuple_SetItem(pTuple, 2, Py_None);
        }
        //printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        delete res_list[j];
    }
    return oplist;
}

extern "C" PyObject * getall_prx(void* p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_content_all(root, content, res_list);

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
    return oplist;
}
extern "C" PyObject * get_prefix_phrases_prx(void *p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_content_prefix_phrases(root, content, res_list);

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
    return oplist;
}
extern "C" PyObject * get_suffix_phrases_prx(void *p, const char *content){
    Next* root = (Next *)p;
    vector<ResInfo *> res_list; res_list.clear();
    get_content_suffix_phrases(root, content, res_list);

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
    res_list.clear();
    return oplist;
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
extern "C" const PyObject * get_kv_prx(void *p, const char* content){
    Next* root = (Next *)p;
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)){
        Py_INCREF(Py_None);
        return Py_None;
    }
    if(candidate_words.size() == 0){
        Py_INCREF(Py_None);
        return Py_None;
    }
    Next * endp = get_phrase_endp(root, candidate_words.begin(), candidate_words.end());
    if(endp == NULL){
        Py_INCREF(Py_None);
        return Py_None;
    }
    const OInfo *oinfo = get_node_oinfo(endp);
    if(oinfo == NULL){
        Py_INCREF(Py_None);
        return Py_None;
    }
    else if(oinfo->type == 0){
        Py_INCREF((PyObject *)oinfo->p);
        return (PyObject *)oinfo->p;
    }else{
        throw("oinfo->type not python obj");
    }
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

// g++ -fPIC token2map.cpp -I/usr/local/app/service/virtualenvs/NLP/include/python2.7 -shared -o token2map_lib.so
int main(){
    while(true){
        Next *root = get_root();
        OInfo *pp[10];
        void *tt[10];
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

        vector<ResInfo *> res_list;
        res_list.clear();
        cut_content_all(root, "121", res_list);
        for(uint32_t j = 0; j < res_list.size(); j++){
            printf("%s, %d ++++\n", res_list[j]->phrase.c_str(), res_list[j]->position);
            delete res_list[j];
        }
        cout<<remove(root, "121")<<endl;
        cout<<remove(root, "124")<<endl;
        cout<<remove(root, "523")<<endl;
        cout<<remove(root, "129")<<endl;
        cout<<remove(root, "8")<<endl;
        cout<<remove(root, "0")<<endl;

        res_list.clear();
        cut_content_all(root, "121", res_list);
        for(uint32_t j = 0; j < res_list.size(); j++){
            printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
            delete res_list[j];
        }
        cout<<"insert end"<<endl;
        free_tree(root);
        free(root);
        root = NULL;
        for(int i = 0; i < 10; i++){
            //void * tt = const_cast<void *>(pp[i]->p);
            free(tt[i]);
        }
        cout<<"end"<<endl;
    }
    /*
    Next *p;
    Next next = Next();
    Node_1 node1 = Node_1();
    node1.word = 23;
    p->next = &node1;
    Node_1 *pp = (Node_1 *)(p->next);
    cout<<pp->word<<endl;
    Node_3 node3 = Node_3();
    node3.words[0] = 22;
    p->next = &node3;
    Node_3 *tt = (Node_3 *)(p->next);
    cout<<tt->words[0]<<endl;
    */
    return 0;
}
