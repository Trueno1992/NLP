#include <iostream>
#include <algorithm>
#include <functional>
#include <queue>
#include <vector>
#include <queue>
#include <map>
using namespace std;

struct node{
    int num;
};

/*
bool operator <(const node *a, const node *b)const{
    return a.num < b.num;
};
*/

struct cmp
{
    template <typename T, typename U>
    bool operator()(T const &left, U const &right)
    {
    // 以second比较。输出结果为Second较大的在前Second相同时，先进入队列的元素在前。
        if (left.first.second < right.first.second) return true;
        return false;
    }
};

int main(){
    map<int, int> m;
    m[2] = 2;
    m[3] = 3;
    vector<pair<int, int> > v(m.begin(), m.end());
    int i = 0;
    if((++i) >= 1) cout<<i<<endl;
    cout<<i<<endl;
    /*
    priority_queue<pair<pair<int, int>, string>, vector<pair<pair<int, int>, string> >, cmp> queue;
    queue.push(make_pair(make_pair(2, 3), "111"));
    queue.push(make_pair(make_pair(1, 4), "222"));
    queue.push(make_pair(make_pair(3, 2), "333"));
    queue.push(make_pair(make_pair(8, 1), "444"));
    queue.push(make_pair(make_pair(7, 5), "444"));
    while(queue.size() != 0){
        pair<pair<int, int>, string> node = queue.top(); queue.pop();
        cout<<node.first.first<<" "<<node.first.second<<" "<<node.second<<endl;
    }
    */
    /*
    int a = 1, b = 2, c=3, d=4;
    vector<int *> vec; vec.clear();
    int *p = &a;
    vec.push_back(p);
    p = &b;
    vec.push_back(p);
    p = &c;
    vec.push_back(p);
    p = &d;
    vec.push_back(p);
    for(int i = 0; i < vec.size(); i++)
        cout<<*vec[i]<<" ";
    cout<<endl;
    reverse(vec.begin(), vec.end());
    for(int i = 0; i < vec.size(); i++)
        cout<<*vec[i]<<" ";
    cout<<endl;
    */
    /*
    priority_queue<node *> que;
    node a = {1};
    node b = {3};
    node c = {2};
    node d = {6};
    que.push(&a);
    que.push(&b);
    que.push(&c);
    que.push(&d);
    cout<<que.top()->num<<endl;
    que.pop();
    cout<<que.top()->num<<endl;
    que.pop();
    cout<<que.top()->num<<endl;
    que.pop();
    cout<<que.top()->num<<endl;
    */
    return 0;
}
