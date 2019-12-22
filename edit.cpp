#include"Python.h"
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<wchar.h>
#include<string.h>
#include<cstring>
#include<locale.h>
#include<vector>
#include<algorithm>
#include<stdint.h>

using namespace std;


struct Node{
    int pre_i;
    int pre_j;
    char action;
};


bool Utf8ToUnicode32(const string& str, vector<uint32_t>& vec) {
  uint32_t tmp;
  vec.clear();
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

/*
    因为状态转移的时候，只需要用到上一个状态的dp[i-1][j-1] 和 dp[i-1][j]，所以保留这两个值，所以一维数组就够了，简单粗暴
*/
static int get_edit_dis_fast(const char *str1, const char *str2){
    if(strlen(str1) == 0)return strlen(str2);
    if(strlen(str2) == 0)return strlen(str1);

    unsigned int *dp = (unsigned int *)malloc((strlen(str2) + 1) * sizeof(unsigned int));

    for(unsigned int j = 0; j < strlen(str2) + 1; j++)dp[j] = j;

    for(unsigned int i = 0; i < strlen(str1); i++){
        unsigned int pre__i__j = (i == 0? dp[0]: (1<<31)), pre__i_j = dp[1];
        for(unsigned int j = 0; j < strlen(str2); j++){
            unsigned int min_val = min(min(pre__i__j + (str1[i] == str2[j]? 0: 1), ((j == 0? (1<<31):dp[j]) + 1)), pre__i_j + 1);
            pre__i__j = pre__i_j; pre__i_j = dp[j + 2];
            dp[j + 1] = min_val;
        }
    }
    int edit_dis = dp[strlen(str2)]; free(dp); dp=NULL;
    return edit_dis;
}


static const char * get_edit_path_fast(const char *str1, const char *str2){
    unsigned int **dp = (unsigned int **)malloc((strlen(str1) + 1) * sizeof(unsigned int *));
    Node **path = (Node **)malloc((strlen(str1) + 1) * sizeof(Node *));
    for(unsigned int i = 0; i < strlen(str1) + 1; i++){
        dp[i] = (unsigned int *)malloc((strlen(str2) + 1) * sizeof(unsigned int));
        path[i] = (Node *)malloc((strlen(str2) + 1) * sizeof(Node));
        for(unsigned int j = 0; j < strlen(str2) + 1; j++){
            path[i][j].pre_i = -1;
            path[i][j].pre_j = -1;
            path[i][j].action = '4';
            dp[i][j] = (1<<31);
        }
    }
    for(unsigned int j = 0; j < strlen(str2) + 1; j++){
        dp[0][j] = j;
        path[0][j].pre_i = 0;
        path[0][j].pre_j = j - 1;
        path[0][j].action = '3';
    }

    for(unsigned int i = 0; i < strlen(str1); i++){
        for(unsigned int j = 0; j < strlen(str2); j++){
            unsigned int _i = i + 1, _j = j + 1;
            unsigned int min_val = dp[_i - 1][_j - 1] + (str1[i] == str2[j]? 0: 1);
            if(min_val <= dp[_i][_j - 1] + 1 && min_val <= dp[_i - 1][_j] + 1){
                path[_i][_j].action = (str1[i] == str2[j]? '0': '1');
                path[_i][_j].pre_i = _i - 1;
                path[_i][_j].pre_j = _j - 1;
            }else if(dp[_i][_j - 1] + 1 <= min_val && dp[_i][_j - 1] + 1 <= dp[_i - 1][_j] + 1){
                path[_i][_j].action = '3';
                path[_i][_j].pre_i = _i;
                path[_i][_j].pre_j = _j - 1;
            }else{
                path[_i][_j].action = '2';
                path[_i][_j].pre_i = _i - 1;
                path[_i][_j].pre_j = _j;
            }
            dp[_i][_j] = min(min(min_val, dp[_i][_j - 1] + 1), dp[_i - 1][_j] + 1);
        }
    }
    int i = strlen(str1), j = strlen(str2); vector<string>res_vec; res_vec.clear();
    while(i >= 0 and j >= 0){
        char str_i[26]; sprintf(str_i, "%d", i);
        char str_j[26]; sprintf(str_j, "%d", j);
        res_vec.push_back(string(str_i) + '-' + string(str_j) + '-' + path[i][j].action);
        i = path[i][j].pre_i;
        j = path[i][j].pre_j;
    }
    reverse(res_vec.begin(), res_vec.end());
    string res_path = "";
    for(vector<string>::iterator it = res_vec.begin(); it != res_vec.end(); it++){
        if(res_path.size() != 0) res_path += '\n';
        res_path.append(*it);
    }
    //cout<<str1<<"\n"<<str2<<"\n"<<dp[strlen(str1)][strlen(str2)]<<"\n"<<res_path<<endl<<endl;
    char str_dis[26]; sprintf(str_dis, "%d", dp[strlen(str1)][strlen(str2)]);
    for(unsigned int i = 0; i < strlen(str1) + 1; i++){
        free(dp[i]); dp[i] = NULL;
        free(path[i]); path[i] = NULL;
    }
    free(dp); dp=NULL;
    free(path); path=NULL;
    return (string(str_dis) + '\n' + res_path).c_str();
}


const char * get_edit_path(const char *str1, const char *str2, int &edit_path){
    vector<unsigned int> vec1; Utf8ToUnicode32(str1, vec1);
    vector<unsigned int> vec2; Utf8ToUnicode32(str2, vec2);

    if(vec1.size() == 0){
        edit_path = vec2.size(); string res_path = "";
        for(unsigned int j = 0; j < vec2.size(); j++){
            if(res_path.size() != 0) res_path += '\n';
            char str_j[26]; sprintf(str_j, "%d", j + 1);
            res_path.append(string("0") + '-' + string(str_j) + '-' + "3");
        }
        return res_path.c_str();
    }

    if(vec2.size() == 0){
        edit_path = vec1.size(); string res_path = "";
        for(unsigned int i = 0; i < vec1.size(); i++){
            if(res_path.size() != 0) res_path += '\n';
            char str_i[26]; sprintf(str_i, "%d", i + 1);
            res_path.append(string(str_i) + '-' + string("0") + '-' + "2");
        }
        return res_path.c_str();
    }


    unsigned int **dp = (unsigned int **)malloc((vec1.size() + 1) * sizeof(unsigned int *));
    Node **path = (Node **)malloc((vec1.size() + 1) * sizeof(Node *));
    for(unsigned int i = 0; i < vec1.size() + 1; i++){
        dp[i] = (unsigned int *)malloc((vec2.size() + 1) * sizeof(unsigned int));
        path[i] = (Node *)malloc((vec2.size() + 1) * sizeof(Node));
        for(unsigned int j = 0; j < vec2.size() + 1; j++){
            path[i][j].pre_i = -1;
            path[i][j].pre_j = -1;
            path[i][j].action = '4';
            dp[i][j] = (1<<31);
        }
    }
    for(unsigned int j = 0; j < vec2.size() + 1; j++){
        dp[0][j] = j;
        path[0][j].pre_i = 0;
        path[0][j].pre_j = j - 1;
        path[0][j].action = '3';
    }
    for(unsigned int i = 0; i < vec1.size(); i++){
        for(unsigned int j = 0; j < vec2.size(); j++){
            unsigned int _i = i + 1, _j = j + 1;
            unsigned int min_val = dp[_i - 1][_j - 1] + (vec1[i] == vec2[j]? 0: 1);
            if(min_val <= dp[_i][_j - 1] + 1 && min_val <= dp[_i - 1][_j] + 1){
                path[_i][_j].action = (vec1[i] == vec2[j]? '0': '1');
                path[_i][_j].pre_i = _i - 1;
                path[_i][_j].pre_j = _j - 1;
            }else if(dp[_i][_j - 1] + 1 <= min_val && dp[_i][_j - 1] + 1 <= dp[_i - 1][_j] + 1){
                path[_i][_j].action = '3';
                path[_i][_j].pre_i = _i;
                path[_i][_j].pre_j = _j - 1;
            }else{
                path[_i][_j].action = '2';
                path[_i][_j].pre_i = _i - 1;
                path[_i][_j].pre_j = _j;
            }
            dp[_i][_j] = min(min(min_val, dp[_i][_j - 1] + 1), dp[_i - 1][_j] + 1);
        }
    }
    int i = vec1.size(), j = vec2.size(); vector<string>res_vec; res_vec.clear();
    while(i >= 0 and j > 0){
        char str_i[26]; sprintf(str_i, "%d", i);
        char str_j[26]; sprintf(str_j, "%d", j);
        res_vec.push_back(string(str_i) + '-' + string(str_j) + '-' + path[i][j].action);
        int _i = path[i][j].pre_i;
        int _j = path[i][j].pre_j;
        i = _i; j = _j;
    }
    reverse(res_vec.begin(), res_vec.end());
    string res_path = "";
    for(vector<string>::iterator it = res_vec.begin(); it != res_vec.end(); it++){
        if(res_path.size() != 0) res_path += '\n';
        res_path.append(*it);
    }
    //cout<<str1<<"\n"<<str2<<"\n"<<dp[vec1.size()][vec2.size()]<<"\n"<<res_path<<endl<<endl;
    edit_path = dp[vec1.size()][vec2.size()];
    for(unsigned int i = 0; i < vec1.size() + 1; i++){
        free(dp[i]); dp[i] = NULL; free(path[i]); path[i] = NULL;
    }
    free(dp); dp=NULL; free(path); path=NULL;
    return res_path.c_str();
}


int get_edit_dis(const char *str1, const char *str2){
    vector<unsigned int> vec1; Utf8ToUnicode32(str1, vec1);
    vector<unsigned int> vec2; Utf8ToUnicode32(str2, vec2);

    if(vec1.size() == 0)return vec2.size();
    if(vec2.size() == 0)return vec1.size();

    unsigned int **dp = (unsigned int **)malloc((vec1.size() + 1) * sizeof(unsigned int *));
    for(unsigned int i = 0; i < vec1.size() + 1; i++){
        dp[i] = (unsigned int *)malloc((vec2.size() + 1) * sizeof(unsigned int));
    }
    for(unsigned int j = 0; j < vec2.size() + 1; j++)dp[0][j] = j;

    for(unsigned int i = 0; i < vec1.size(); i++){
        dp[i+1][0] = (1<<31);
        for(unsigned int j = 0; j < vec2.size(); j++){
            unsigned int _i = i + 1, _j = j + 1;
            unsigned int min_val = dp[_i - 1][_j - 1] + (vec1[i] == vec2[j]? 0: 1);
            dp[_i][_j] = min(min(min_val, dp[_i][_j - 1] + 1), dp[_i - 1][_j] + 1);
        }
    }
    int edit_dis = dp[vec1.size()][vec2.size()];
    for(unsigned int i = 0; i < vec1.size() + 1; i++){
        free(dp[i]); dp[i] = NULL;
    }
    free(dp); dp=NULL;
    return edit_dis;
}


PyObject* wrap_edit_dis(PyObject* self, PyObject* args){
    char *str1, *str2;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    int dis = get_edit_dis(str1, str2);
    return Py_BuildValue("i", dis);
}


PyObject* wrap_edit_path(PyObject* self, PyObject* args){
    char *str1, *str2; int dis;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    const char *path = get_edit_path(str1, str2, dis);
    return Py_BuildValue("is", dis, path);
}


PyObject* wrap_edit_dis_fast(PyObject* self, PyObject* args){
    char *str1, *str2;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    int dis = get_edit_dis_fast(str1, str2);
    return Py_BuildValue("i", dis);
}


PyObject* wrap_edit_path_fast(PyObject* self, PyObject* args){
    char *str1, *str2;
    if (!PyArg_ParseTuple(args, "ss", &str1, &str2)) return NULL;
    const char *path = get_edit_path_fast(str1, str2);
    return Py_BuildValue("s", path);
}


static PyMethodDef editMethods[] = {
        {"edit_dis", wrap_edit_dis, METH_VARARGS, "doc: get_edit_dis"},
        {"edit_path", wrap_edit_path, METH_VARARGS, "doc: get_edit_dis"},
        {"edit_dis_fast", wrap_edit_dis_fast, METH_VARARGS, "doc: get_edit_dis"},
        {"edit_path_fast", wrap_edit_path_fast, METH_VARARGS, "doc: get_edit_dis"},
        {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC initLD(void) {
        (void) Py_InitModule("LD", editMethods);
}


int main(){
    int dis;
    cout<<get_edit_path("发的尽快是", "放假啊上看到", dis)<<endl;
    cout<<dis<<endl;
    return 0;
}
