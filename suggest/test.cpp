# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<unistd.h>
# include<string.h>
# include<map>
# include<vector>
# include<stdint.h>
# include<queue>
# include<algorithm>
# include<queue>
# include<utility>
# include<pthread.h>
# include<StrUtils.h>
# include<suggest2.h>
//# include<mutex>
using namespace std;

int main(){
    NodeArr<int, int, 3> * node = new NodeArr<int, int, 3>;
    node->son_words[2] = 10;
    node->resize(8);
    node->son_words[6] = 10;
}
