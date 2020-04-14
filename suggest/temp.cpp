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
int main(){
    //node2<int, int> *n2 = new node2<int, int>(1, 2);
    //node1<int, int> *n1 = new node1<int, int>(n2);
    //cout<<n1->w<<endl;
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
    return 0;
}
