/*
doc: 计算编辑距离的多种尝试，静态或者动态内存开辟， 一维数组实现，二维数组实现。保留编辑操作等。
author: wulangzhou
email: 1207175256@qq.com
*/

#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<locale.h>
#include<wchar.h>
#include<string.h>
#include<cstring>
#include<locale.h>
#include<vector>
#include<stdint.h>
#include<algorithm>

using namespace std;


struct Node{
    unsigned int pre_i;
    unsigned int pre_j;
    char action;
};


void gen_random_string(char *p, int len_s){
    for(unsigned int i = 0; i < len_s; i++){
        p[i] = char((rand() % 26) + 97);
    }
    p[len_s] = '\0';
}


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
   哎，好久没有写c代码了，来一个二维数组实现，简单粗暴
*/
int get_edit_dis2(const char *str1, const char *str2){
    if(strlen(str1) == 0)return strlen(str2);
    if(strlen(str2) == 0)return strlen(str1);

    unsigned int **dp = (unsigned int **)malloc((strlen(str1) + 1) * sizeof(unsigned int *));
    for(unsigned int i = 0; i < strlen(str1) + 1; i++){
        dp[i] = (unsigned int *)malloc((strlen(str2) + 1) * sizeof(unsigned int));
    }
    for(unsigned int j = 0; j < strlen(str2) + 1; j++)dp[0][j] = j;

    for(unsigned int i = 0; i < strlen(str1); i++){
        dp[i+1][0] = (1<<31);
        for(unsigned int j = 0; j < strlen(str2); j++){
            unsigned int _i = i + 1, _j = j + 1;
            unsigned int min_val = dp[_i - 1][_j - 1] + (str1[i] == str2[j]? 0: 1);
            dp[_i][_j] = min(min(min_val, dp[_i][_j - 1] + 1), dp[_i - 1][_j] + 1);
        }
    }
    int edit_dis = dp[strlen(str1)][strlen(str2)];
    for(unsigned int i = 0; i < strlen(str1) + 1; i++){
        free(dp[i]); dp[i] = NULL;
    }
    free(dp); dp=NULL;
    return edit_dis;
}


/*
    因为状态转移的时候，只需要用到上一个状态的dp[i-1][j-1] 和 dp[i-1][j]，所以保留这两个值，所以一维数组就够了，简单粗暴
*/
int get_edit_dis1(const char *str1, const char *str2){
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

/*
   准备用一维数组实现，还没来得及写
 */
const char * get_edit_path(const char *str1, const char *str2){
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
    while(i >= 0 and j > 0){
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


const char * get_edit_path2(const char *str1, const char *str2){
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
    while(i >= 0 and j > 0){
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


const char * get_edit_path3(const char *str1, const char *str2){
    vector<unsigned int> vec1; Utf8ToUnicode32(str1, vec1);
    vector<unsigned int> vec2; Utf8ToUnicode32(str2, vec2);
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
        i = path[i][j].pre_i;
        j = path[i][j].pre_j;
    }
    reverse(res_vec.begin(), res_vec.end());
    string res_path = "";
    for(vector<string>::iterator it = res_vec.begin(); it != res_vec.end(); it++){
        if(res_path.size() != 0) res_path += '\n';
        res_path.append(*it);
    }
    //cout<<str1<<"\n"<<str2<<"\n"<<dp[vec1.size()][vec2.size()]<<"\n"<<res_path<<endl<<endl;
    char str_dis[26]; sprintf(str_dis, "%d", dp[vec1.size()][vec2.size()]);
    for(unsigned int i = 0; i < vec1.size() + 1; i++){
        free(dp[i]); dp[i] = NULL;
        free(path[i]); path[i] = NULL;
    }
    free(dp); dp=NULL;
    free(path); path=NULL;
    return (string(str_dis) + '\n' + res_path).c_str();
}


int get_edit_dis3(const char *str1, const char *str2){
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


int get_edit_dis5(const char *str1, const char *str2){
    vector<unsigned int> vec1; Utf8ToUnicode32(str1, vec1);
    vector<unsigned int> vec2; Utf8ToUnicode32(str2, vec2);

    if(vec1.size() == 0)return vec2.size();
    if(vec2.size() == 0)return vec1.size();

    unsigned int *dp = (unsigned int *)malloc((vec2.size() + 1) * sizeof(unsigned int));
    for(unsigned int j = 0; j < vec2.size() + 1; j++)dp[j] = j;
    for(unsigned int i = 0; i < vec1.size(); i++){
        unsigned int pre__i__j = (i == 0? dp[0]: (1<<31)), pre__i_j = dp[1];
        for(unsigned int j = 0; j < vec2.size(); j++){
            unsigned int min_val = min(min(pre__i__j + (vec1[i] == vec2[j]? 0: 1), ((j == 0? (1<<31):dp[j]) + 1)), pre__i_j + 1);
            pre__i__j = pre__i_j; pre__i_j = dp[j + 2];
            dp[j + 1] = min_val;
        }
    }
    int edit_dis = dp[vec2.size()]; free(dp); dp=NULL;
    return edit_dis;
}


unsigned int dp4[1000000];
int get_edit_dis4(const char *str1, const char *str2){
    vector<unsigned int> vec1; Utf8ToUnicode32(str1, vec1);
    vector<unsigned int> vec2; Utf8ToUnicode32(str2, vec2);

    if(vec1.size() == 0)return vec2.size();
    if(vec2.size() == 0)return vec1.size();

    if(vec2.size() >= 1000000){
        throw "input string is to long ,is mush < 1000000";
    }
    unsigned int *dp = dp4;
    for(unsigned int j = 0; j < vec2.size() + 1; j++)dp[j] = j;
    for(unsigned int i = 0; i < vec1.size(); i++){
        unsigned int pre__i__j = (i == 0? dp[0]: (1<<31)), pre__i_j = dp[1];
        for(unsigned int j = 0; j < vec2.size(); j++){
            unsigned int min_val = min(min(pre__i__j + (vec1[i] == vec2[j]? 0: 1), ((j == 0? (1<<31):dp[j]) + 1)), pre__i_j + 1);
            pre__i__j = pre__i_j; pre__i_j = dp[j + 2];
            dp[j + 1] = min_val;
        }
    }
    return dp[vec2.size()];
}


int main(){
    int test_num = 100000;

    unsigned len_1 = 300 + 1;
    unsigned len_2 = 400 + 1;
    char str1[len_1], str2[len_2];
    gen_random_string(str1, sizeof(str1));
    gen_random_string(str2, sizeof(str2));
    while(test_num--){
        //unsigned len_1 = (rand() % 1000) + 1;
        //unsigned len_2 = (rand() % 1000) + 1;
        //char str1[len_1], str2[len_2];
        //gen_random_string(str1, sizeof(str1));
        //gen_random_string(str2, sizeof(str2));
        //cout<<str1<<endl;
        //cout<<str2<<endl;
        //cout<<(get_edit_dis1(str1, str2) == get_edit_dis4(str1, str2))<<endl;
        cout<<(get_edit_dis4(str1, str2) == get_edit_dis4(str1, str2))<<endl;
    }
    cout<<get_edit_dis1("lefszbexrampetvhqnddjeqvuygpn", "kazqfrpjvoaxdpcwmjobmskskfojnewxgxnnofwltwjwnnvbwjckdmeouuzhyvhgvwujbqxxpitcvograid")<<endl;
    cout<<get_edit_dis2("lefszbexrampetvhqnddjeqvuygpn", "kazqfrpjvoaxdpcwmjobmskskfojnewxgxnnofwltwjwnnvbwjckdmeouuzhyvhgvwujbqxxpitcvograid")<<endl;

    cout<<get_edit_dis1("1", "")<<endl;
    cout<<get_edit_dis1("1", "2")<<endl;
    cout<<get_edit_dis1("1", "1")<<endl;
    cout<<get_edit_dis1("1", "11")<<endl;
    cout<<get_edit_dis1("12", "11")<<endl;
    cout<<get_edit_dis1("fjasdkfjaskdfjaksdfjaksdfjkasdjfkasdjfkasdfjkasdjfk", "wulangzhou")<<endl;
    cout<<get_edit_dis2("fjasdkfjaskdfjaksdfjaksdfjkasdjfkasdjfkasdfjkasdjfk", "wulangzhou")<<endl;
    //setlocale(LC_CTYPE, "zh_CN.utf8");
    vector<unsigned int> res;
    cout<<Utf8ToUnicode32("fsajk大戟科", res)<<endl;
    cout<<"res size="<<res.size()<<endl;
    cout<<Utf8ToUnicode32("fsajk", res)<<endl;
    cout<<"res size="<<res.size()<<endl;
    cout<<get_edit_path3("发几块", "发生口块")<<endl;
    cout<<get_edit_path3("", "ff")<<endl;
    cout<<get_edit_path3("f", "f")<<endl;
    cout<<get_edit_path3("f", "ff")<<endl;
    cout<<get_edit_path3("f2", "ff")<<endl;
    cout<<get_edit_path3("fjasdkfjaskdfjaksdfjaksdfjkasdjfkasdjfkasdfjkasdjfk", "wulangzhou")<<endl;
    cout<<get_edit_path3("aou", "wulangzhou")<<endl;
    cout<<get_edit_path3("maxiaoyu", "wulangzhou")<<endl;
    cout<<get_edit_path3("fads", "fsdfas")<<endl;
    cout<<get_edit_path3("fads", "faads")<<endl;
    cout<<get_edit_path3("fads", "fadss")<<endl;
    cout<<get_edit_path3("fads", "uads")<<endl;
    return 0;
}
