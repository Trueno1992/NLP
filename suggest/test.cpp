# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<unistd.h>
# include<string.h>
# include<map>
# include<time.h>
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
    Tree<int, int, 15> *tree = new Tree<int, int, 15>(&tt);

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

void test2(){
    std::vector<uint8_t> tt; tt.clear();
    Tree<string, string, 15> *tree = new Tree<string, string, 15>(&tt);
    cout<<"fd"<<endl;
    tree->insert("110", "11", "name"); 
    tree->insert("111", "22", "name"); 
    tree->insert("112", "33", "name"); 
    tree->insert("113", "44", "name"); 
    tree->insert("114", "55", "name"); 
    tree->insert("115", "66", "name"); 
    std::vector<ResInfo<string, string> > res_vec;
    tree->get_suffix_info("11", res_vec, 3);
    for(uint32_t i=0; i <res_vec.size(); i++){
        cout<<res_vec[i].phrase<<endl;
        cout<<res_vec[i].weight<<endl;
        cout<<res_vec[i].info<<endl;
        cout<<endl;
    }
}

//g++ test.cpp suggest2.h utils/StrUtils.o -o a.out -I./ -I./utils/
//ps -ef |grep a.out|grep -v grep|awk -F ' ' '{print $2}'|xargs htop -p
int main(){
    //test_son_memory_release();
    std::vector<uint8_t> tt; tt.clear();
    Tree<int, string, 15> *tree = new Tree<int, string, 15>(&tt);
    while(true){
    tree->insert("110", 1, "name"); 
    tree->insert("111", 2, "name"); 
    tree->insert("112", 3, "name"); 
    tree->insert("113", 4, "name"); 
    tree->insert("114", 5, "name"); 
    tree->insert("115", 6, "name"); 
    tree->insert("116", 1, "name"); 
    tree->insert("117", 8, "name"); 
    cout<<"insert end"<<endl;
    tree->remove("117");
    tree->remove("116");
    tree->remove("115");
    tree->remove("114");
    tree->remove("113");
    tree->remove("112");
    tree->remove("111");
    tree->remove("110");
    cout<<"remove end"<<endl;
    int t = 10000; vector<char *> vec; vec.clear();
    while(t--){
        char *p = get_random_str((rand() % 50) + 1);
        tree->insert(p, rand() % 1000, "name");
        vec.push_back(p);
    }
    for(int i = 0; i < vec.size(); i++){
        tree->remove(vec[i]);
        free(vec[i]);
    }
    vec.clear();
    cout<<"suffix_count="<<tree->get_suffix_count("11")<<endl;
    sleep(1);
    }

    tree->insert("117", 8, "name"); 
    tree->insert("210", 2, "name"); 
    tree->insert("310", 3, "name"); 
    tree->insert("410", 4, "name"); 
    tree->insert("510", 5, "name"); 
    tree->insert("610", 6, "name"); 
    tree->insert("710", 7, "name"); 
    tree->insert("810", 8, "name"); 
    tree->insert("910", 9, "name"); 
    cout<<tree->get_suffix_count("11")<<endl;

    std::vector<ResInfo<int, string> > res_vec;
    tree->get_suffix_info("11", res_vec, 3);
    for(uint32_t i=0; i <res_vec.size(); i++){
        cout<<res_vec[i].phrase<<endl;
        cout<<res_vec[i].weight<<endl;
        cout<<res_vec[i].info<<endl;
        cout<<endl;
    }
    /*
    while(true){
        Tree<int, string, 15> *tree = new Tree<int, string, 15>(tt, 0);
        int t = 20000;
        while(t--){
            char *p = get_random_str((rand() % 50) + 1);
            cout<<t<<" "<<p<<endl;
            tree->insert(p, rand() % 1000, "name");
            cout<<(tree->get_endp(p) == NULL)<<endl;
            free(p);
        }
        delete tree;
    }
    */
    return 0;
}
