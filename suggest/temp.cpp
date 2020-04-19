#include<iostream>
# include <map>
# include <vector>
# include <stdio.h>
# include <iostream>
# include <pthread.h>
# include <unistd.h>    // sleep
# include <string.h>
# include <stdint.h>    // uint32_t
# include <utility>     // 包含pair
# include <queue>
# include <algorithm>


using namespace std;

template<class W, class T>
class node2{
public:
    node2(W w, T t){
        w=w;
        t=t;
    }
public:
    W w;
    T t;
};

template<class W, class T>
class node1{
public:
    node1(node2<W, T> *n2){
        w=n2->w;
        t=n2->t;
    }
public:
    W w;
    T t;
};
class a{
public:
    virtual ~a(){
        cout<<"a"<<endl;
    }
    int c;
};
class b: public a{
public:
    ~b(){
        cout<<"b"<<endl;
    }
};

template<class W> bool a_gt_b(const W &a, const W &b){
    if(a >= b) return true;
    return false;
}


template<class W, bool (*gt)(const W &a, const W &b)>
class Node{
public:
    bool a_gt_b_test(W a, W b){
        return gt(a, b);
    }
public:
    int a;
};

template<class W, bool (*gt)(const W &a, const W &b)=a_gt_b>
class SonNode: public Node<W, gt>{
public:
    SonNode(){
    }
    //bool compare(W, W);
public:
    int c;
};

/*
template<class W, bool (*gt)(const W &a, const W &b)=a_gt_b>
bool SonNode<W, gt>::compare(W a, W b){
    return gt(a, b);
}
*/


int main(){
    //node2<int, int> *n2 = new node2<int, int>(1, 2);
    //node1<int, int> *n1 = new node1<int, int>(n2);
    //cout<<n1->w<<endl;
    /*
    try{
        int a = 10;
        int k = 1/0;
    }catch(const char * e){
        cout<<e<<endl;
    }catch(const std::exception &e){
        cout<<e.what()<<endl;
    }catch(...){
        throw;
    }
    */
    exit(0);
    SonNode<int> son;
    //cout<<son.compare(4, 5)<<endl;
    //cout<<son.compare(4, -5)<<endl;
    cout<<"fds"<<endl;
    return 0;
}
