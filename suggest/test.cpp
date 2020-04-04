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

template<class T=int, int son_count=20>
class Info{
    T oinfo;
    int sons[son_count];
};
// g++ test.cpp suggest2.cpp ./utils/StrUtils.o -o a.out -I./utils -I ./

template<class W, class T>
class BASE{
public:
    void test(){
    }

private:
    class B{
    public:
        int test();
    public:
        W w;
        T t;
    };
};

template<class W, class T>
int BASE<W,T>::B::test(){
    cout<<3<<endl;
}

void test_son_memory_release(){
    int arr_size = 1;
    NodeArr<int, int> *node = new NodeArr<int, int>(arr_size);

    std::vector<uint8_t> tt; tt.clear();
    Tree<int, int, 15> *tree = new Tree<int, int, 15>(tt, 0);

    while(true){
        Son<int, int> * son = new Son<int, int>(); son->to_arrSon(1, 2);
        delete son;

        arr_size += 1;
        node->resize(arr_size);

        if(arr_size >= 120){
            arr_size = 1;
            NodeMap<int, int> *nodemap = tree->arr2map(node);
            delete nodemap;
            node = new NodeArr<int, int>(arr_size);
        }

        Son<int, int> * son2 = new Son<int, int>(); son2->to_mapSon(2);
        delete son2;
    }
}
int main(){
    //test_son_memory_release();
    std::vector<uint8_t> tt; tt.clear();
    while(true){
        Tree<int, string, 15> *tree = new Tree<int, string, 15>(tt, 0);
        int t = 20000;
        while(t--){
            char *p = get_random_str((rand() % 5) + 1);
            cout<<t<<" "<<*p<<endl;
            tree->insert(p, rand() % 1000, "name");
            free(p);
        }
        delete tree;
    }
    return 0;
}
