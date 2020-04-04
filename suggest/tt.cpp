#include<iostream>
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

int main(){
    node2<int, int> *n2 = new node2<int, int>(1, 2);
    node1<int, int> *n1 = new node1<int, int>(n2);
    cout<<n1->w<<endl;
    return 0;
}
