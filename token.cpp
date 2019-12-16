# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<map>
# include<vector>
#include<stdint.h>
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


void Unicode32ToUtf8(uint32_t &ui, string& res) {
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


struct Next{
    void *next;
    uint8_t type;
};


struct Node_1{
    bool is_end;
    uint32_t word;
    Next *next;
};


struct Node_3{
    bool is_end;
    uint32_t words[3];
    Next *nexts[3];
};


struct Node_6{
    bool is_end;
    uint32_t words[6];
    Next *nexts[6];
};


struct Nodes{
    bool is_end;
    map<uint32_t, Next *> *next_map;
};


bool insert(Next* root, const char* content){
    int dep = 0; Next* p = root; Next** parent = &root;

    //printf("%d --\n", root->type);

    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return false;

    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;

    map<uint32_t, Next *>::iterator ik;
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++){
        dep += 1;
        //printf("%d, %d\n", p->type, *it);
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik != pn->next_map->end()){
                parent = &ik->second;
                p = ik->second;
            }else{
                Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                tmp_node->is_end = false;
                tmp_node->word = 0;
                tmp_node->next = NULL;

                Next * tmp_next = (Next *)malloc(sizeof(Next));
                tmp_next->next = tmp_node;
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
                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    p1->next = (Next *)malloc(sizeof(Next));
                    p1->next->next = tmp_node;
                    p1->next->type = 1;
                    p1->word = *it;

                    parent = &(p1->next);
                    p = p1->next;
                }else{
                    Node_3 * new_node = (Node_3 *)malloc(sizeof(Node_3));
                    new_node->is_end = false;
                    for(unsigned int i = 0; i < 3; i ++){
                        new_node->words[i] = 0;
                        new_node->nexts[i] = NULL;
                    }

                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    new_node->words[0] = p1->word;
                    new_node->nexts[0] = p1->next;

                    new_node->nexts[1] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[1]->next = tmp_node;
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
                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    p3->nexts[i] = (Next *)malloc(sizeof(Next));
                    p3->nexts[i]->next = tmp_node;
                    p3->nexts[i]->type = 1;
                    p3->words[i] = *it;

                    parent = &(p3->nexts[i]);
                    p = p3->nexts[i];
                    find_position = true;
                    break;
                }
                if(!find_position){
                    Node_6 * new_node = (Node_6 *)malloc(sizeof(Node_6));
                    new_node->is_end = false;
                    for(unsigned int i = 0; i < 6; i ++){
                        new_node->words[i] = 0;
                        new_node->nexts[i] = NULL;
                    }

                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    for(unsigned int i = 0; i < 3; i++){
                        new_node->words[i] = p3->words[i];
                        new_node->nexts[i] = p3->nexts[i];
                    }
                    new_node->nexts[3] = (Next *)malloc(sizeof(Next));
                    new_node->nexts[3]->next = tmp_node;
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
                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    p6->nexts[i] = (Next *)malloc(sizeof(Next));
                    p6->nexts[i]->next = tmp_node;
                    p6->nexts[i]->type = 1;
                    p6->words[i] = *it;

                    parent = &(p6->nexts[i]);
                    p = p6->nexts[i];
                    have_position = true;
                    break;
                }
                if(!have_position){
                    Nodes * new_node = (Nodes *)malloc(sizeof(Nodes));
                    new_node->is_end = false;
                    new_node->next_map = new map<uint32_t, Next *>;
                    new_node->next_map->clear();

                    Node_1 * tmp_node = (Node_1 *)malloc(sizeof(Node_1));
                    tmp_node->is_end = false;
                    tmp_node->word = 0;
                    tmp_node->next = NULL;

                    Next *tt_next = (Next *)malloc(sizeof(Next));
                    tt_next->next = tmp_node;
                    tt_next->type = 1;

                    for(unsigned int i = 0; i < 6; i++){
                        (*new_node->next_map)[p6->words[i]] = p6->nexts[i];
                    }
                    (*new_node->next_map)[*it] = tt_next;

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
    if(p->type == 0){
        pn = (Nodes *)p->next;
        pn->is_end=true;
    }else if (p->type == 1){
        p1 = (Node_1 *)p->next;
        p1->is_end=true;
    }else if (p->type == 3){
        p3 = (Node_3 *)p->next;
        p3->is_end=true;
    }else if (p->type == 6){
        p6 = (Node_6 *)p->next;
        p6->is_end=true;
    }
    return true;
}


struct Info{
    string phrase;
    uint32_t position;
};


bool is_node_end(Next *p){
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


/*
return: 从当前位置切分的最长切分位置
*/
uint32_t cut_from(Next* root,
                  vector<uint32_t>:: iterator start,
                  vector<uint32_t>:: iterator end,
                  vector<Info *> &res_list,
                  uint32_t offset_position=0){
    Next* p = root; Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;
    map<uint32_t, Next *>::iterator ik; string res_str = ""; uint32_t max_position = offset_position;
    for(vector<uint32_t>:: iterator it = start; it != end; it++, offset_position++){
        //string str = ""; Unicode32ToUtf8(*it, str);
        //printf("%s ++\n", str.c_str());
        if (p->type == 0){
            pn = (Nodes *)p->next;
            ik = pn->next_map->find(*it);
            if(ik == pn->next_map->end())return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = ik->second;
            if(is_node_end(p)){
                Info *info = new Info();
                info->phrase = res_str;
                info->position = offset_position;
                res_list.push_back(info);
            }
            continue;
        }

        if (p->type == 1){
            p1 = (Node_1 *)p->next;
            if(p1->word != *it)return max_position;

            Unicode32ToUtf8(*it, res_str);
            p = p1->next;
            if(is_node_end(p)){
                Info *info = new Info();
                info->phrase = res_str;
                info->position = offset_position;
                res_list.push_back(info);
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
                    if(is_node_end(p)){
                        Info *info = new Info();
                        info->phrase = res_str.c_str();
                        info->position = offset_position;
                        res_list.push_back(info);
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
                    if(is_node_end(p)){
                        Info *info = new Info();
                        info->phrase = res_str;
                        info->position = offset_position;
                        res_list.push_back(info);
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


void cut_all(Next* root, const char* content, vector<Info *>& res_list){
    vector<uint32_t> candidate_words;
    if(!Utf8ToUnicode32(content, candidate_words)) return;
    uint32_t position = 0; res_list.clear();
    for(vector<uint32_t>:: iterator it = candidate_words.begin(); it != candidate_words.end(); it++, position++){
        cut_from(root, it, candidate_words.end(), res_list, position);
    }
}


void free_tree(Next* p){
    Node_1 *p1; Node_3 *p3; Node_6 *p6; Nodes *pn;

    if(p->type == 0){
        pn = (Nodes *)p->next;
        if(pn->next_map != NULL){
            for(map<uint32_t, Next *>::iterator it=pn->next_map->begin(); it != pn->next_map->end(); it++){
                //string str = ""; uint32_t tmp=it->first; Unicode32ToUtf8(tmp, str);
                //printf("%d, %s-----\n", p->type, str.c_str());
                free_tree(it->second);
                free(it->second);
                it->second = NULL;
            }
        }
        pn->next_map->clear();
        delete pn->next_map;
        pn->next_map = NULL;
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
        return;
    }
    throw("p->type not in (0, 1, 3, 6)");
}


int main(){
    while(true){
        Next *root = (Next *)malloc(sizeof(Next));
        root->next = (Node_1 *)malloc(sizeof(Node_1));
        root->type = 1;
        cout<<insert(root, "121")<<endl;
        cout<<insert(root, "122")<<endl;
        cout<<insert(root, "123")<<endl;
        cout<<insert(root, "124")<<endl;
        cout<<insert(root, "125")<<endl;
        cout<<insert(root, "126")<<endl;
        cout<<insert(root, "127")<<endl;
        cout<<insert(root, "128")<<endl;
        cout<<insert(root, "129")<<endl;

        cout<<insert(root, "133")<<endl;
        cout<<insert(root, "143")<<endl;
        cout<<insert(root, "153")<<endl;
        cout<<insert(root, "163")<<endl;
        cout<<insert(root, "173")<<endl;
        cout<<insert(root, "183")<<endl;

        cout<<insert(root, "19")<<endl;
        cout<<insert(root, "10")<<endl;
        cout<<insert(root, "223")<<endl;
        cout<<insert(root, "323")<<endl;
        cout<<insert(root, "423")<<endl;
        cout<<insert(root, "523")<<endl;
        cout<<insert(root, "623")<<endl;
        cout<<insert(root, "723")<<endl;
        cout<<insert(root, "823")<<endl;
        cout<<insert(root, "82")<<endl;
        cout<<insert(root, "81")<<endl;
        cout<<insert(root, "8")<<endl;

        vector<Info *> res_list;
        cut_all(root, "11", res_list);
        for(uint32_t j = 0; j < res_list.size(); j++)
            printf("%s, %d ----\n", res_list[j]->phrase.c_str(), res_list[j]->position);
        //free_tree(root);
        root = NULL;
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
