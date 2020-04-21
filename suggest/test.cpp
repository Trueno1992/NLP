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
# include<tree.h>
//# include<mutex>
using namespace std;

struct Info{
    int a; int b;
    Info(int a, int b){
        this->a = a; this->b = b;
    }
};

void test_son_memory_release(){
    int arr_size = 1; int num = 0; vector<uint8_t> vec; vec.push_back(1);

    NodeArr<int, int> *resize_node = new NodeArr<int, int>(arr_size);
    while(true){
        Son<int, int> * arrson = new Son<int, int>();
        arrson->to_arrSon(25, 20);
        delete arrson;

        arr_size += 1; resize_node->resize(arr_size);
        cout<<(resize_node->gset_son((rand() % 256) + 20, 8, vec) == NULL)<<endl;
        if(arr_size >= 200){
            arr_size = 1;
            delete resize_node;
            resize_node= new NodeArr<int, int>(arr_size);
        }

        Son<int, int> * mapson= new Son<int, int>();
        mapson->to_mapSon(20);
        delete mapson;
        num += 1;
    }
}

void test_arr2map(){
    int arr_size = 1; int num = 0; vector<uint8_t> vec; vec.push_back(1);
    NodeArr<int, int> *arrnode = new NodeArr<int, int>(arr_size);
    while(true){
        arr_size += 1; arrnode->resize(arr_size);
        cout<<(arrnode->gset_son((rand() % 256) + 20, 8, vec) == NULL)<<endl;
        if(arr_size >= 200){
            arr_size = 1;
            NodeMap<int, int> * mapnode = arr2map<int, int>(arrnode);
            delete mapnode;
            arrnode = new NodeArr<int, int>(arr_size);
        }
        num += 1;
    }
}

void test_strWeight_suffix(){
    Tree<string, string> *tree = new Tree<string, string>();
    tree->insert("110", "11", "name0"); 
    tree->insert("111", "22", "name1"); 
    tree->insert("112", "33", "name2"); 
    tree->insert("113", "44", "name3"); 
    tree->insert("114", "55", "name4"); 
    tree->insert("115", "66", "name5"); 
    tree->insert("116", "77", "name6"); 
    tree->insert("118", "70", "name8"); 
    tree->remove("116"); 
    std::vector<PTWInfo<string, string> > res_vec;
    tree->get_suffix_info("11", res_vec, 3);
    for(uint32_t i=0; i <res_vec.size(); i++){
        cout<<res_vec[i].word<<endl;
        cout<<*res_vec[i].weight<<endl;
        cout<<*res_vec[i].info<<endl;
        cout<<endl;
    }
}

void test_get_suffix_info(){
    int num = 100;
    Tree<int, string> *tree = new Tree<int, string>();
    while(num--){
        int t = 100; vector<char *> vec; vec.clear();
        while(t--){
            char *p = get_random_str((rand() % 50) + 1);
            tree->insert(p, rand() % 1000, "name");
            vec.push_back(p);
        }

        std::vector<PTWInfo<int, string> > res_vec;
        tree->get_suffix_info("aa", res_vec, 3);
        for(uint32_t i=0; i <res_vec.size(); i++){
            cout<<res_vec[i].word<<endl;
            cout<<res_vec[i].len<<endl;
            cout<<*res_vec[i].weight<<endl;
            cout<<*res_vec[i].info<<endl;
            cout<<endl;
        }
        for(int i = 0; i < vec.size(); i++){
            tree->remove(vec[i]);
            free(vec[i]);
        }
        cout<<"suffix_count="<<tree->get_suffix_count("11")<<endl;
    }
    delete tree; 
}

void test_cut_max(){
    Tree<int, string> *tree = new Tree<int, string>();
    while(true){
        int t = 1000; vector<char *> vec; vec.clear();
        while(t--){
            char *p = get_random_str((rand() % 3) + 1);
            tree->insert(p, rand() % 1000, "name");
            vec.push_back(p);
        }
        std::vector<PTInfo<string> > res_vec;
        tree->cut_max("aaaaa", res_vec);
        for(uint32_t i=0; i <res_vec.size(); i++){
            cout<< res_vec[i].word  <<endl;
            cout<< res_vec[i].len   <<endl;
            cout<< res_vec[i].offset<<endl;
            if(res_vec[i].info != NULL){
                cout<<*res_vec[i].info<<endl;
            }else{
                std::vector<PTWInfo<int, string> > suffix_vec;
                tree->get_suffix_info("a", suffix_vec, 10000);
                for(int i = 0; i < suffix_vec.size(); i++){
                    cout<<suffix_vec[i].word<<" suffix"<<endl;
                }
                cout<<"unknow"<<endl;
            }
            cout<<endl;
        }
        for(int i = 0; i < vec.size(); i++){
            tree->remove(vec[i]);
            free(vec[i]);
        }
        cout<<"suffix_count="<<tree->get_suffix_count("11")<<endl;
    }
}

void test_accuracy_get_suffix_info(){
    Tree<int, string> *tree = new Tree<int, string>();
    tree->insert("127", 12,  "name"); 
    tree->insert("130", 23,  "name"); 
    tree->insert("140", 9,   "name"); 
    tree->insert("150", 14,  "name"); 
    tree->insert("160", 55,  "name"); 
    tree->insert("170", 7,   "name"); 
    tree->insert("180", 98,  "name"); 
    tree->insert("190", 29,  "name"); 
    tree->insert("120", 22,  "name"); 
    tree->insert("121", 43,  "name"); 
    tree->insert("122", 11,  "name"); 
    tree->insert("123", 3,   "name"); 
    tree->insert("124", 101, "name"); 
    tree->insert("125", 74,  "name"); 
    tree->insert("126", 23,  "name"); 
    tree->insert("128", 55,  "name"); 
    tree->insert("118", 35,  "name"); 
    tree->remove("1");
    tree->remove("180");
    cout<<"res_size="<<tree->get_suffix_count("11")<<endl<<endl;
    std::vector<PTWInfo<int, string> > res_vec;
    tree->get_suffix_info("1", res_vec, 3);
    for(uint32_t i=0; i <res_vec.size(); i++){
        cout<<"word="<<res_vec[i].word<<endl;
        cout<<"weight="<<*res_vec[i].weight<<endl;
        cout<<"info="<<*res_vec[i].info<<endl;
        cout<<endl;
    }
    delete tree;
}

void test_struct_info(){
    Tree<int, Info> *tree = new Tree<int, Info>();
    while(true){
        Info info1(11, 21);
        Info info2(12, 22);
        Info info3(13, 23);
        Info info4(14, 24);
        Info info5(15, 25);
        Info info6(16, 26);
        Info info7(17, 27);
        Info * info8 = new Info(18, 28);
        tree->insert("2", 1, info2);
        tree->insert("1", 2, info1);
        tree->insert("3", 2, info3);
        tree->insert("4", 2, info4);
        tree->insert("5", 2, info5);
        tree->insert("6", 2, info6);
        tree->insert("7", 2, info7);
        tree->insert("8", 2, *info8);
        delete info8;
        break;
    }
    Info *p = tree->get_info("8");
    cout<<p->a<<" "<<p->b<<endl;

    Info *q = tree->get_info("7");
    cout<<q->a<<" "<<q->b<<endl;
    delete tree;
}

template<class W, class T>
void * update_thread(void *args){
    ConcurrentTree<W, T> * tree = (ConcurrentTree<W, T> *)args;
    try{
        for(int i = 0; i < 10000; i++){
            Info info(i, i + 100);
            tree->insert((string("1") + toString(i)).c_str(), i, info);

            std::vector<PTWInfo<W, T> > res_vec; res_vec.clear();
            int t = 5; while(t--)tree->get_suffix_info("1", res_vec, 3);
            sleep(0.1);
            break;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
template<class W, class T>
void * delete_thread(void *args){
    ConcurrentTree<W, T> * tree = (ConcurrentTree<W, T> *)args;
    try{
        for(int i = 0; i < 10000; i++){
            tree->remove((string("1") + toString(i)).c_str());

            std::vector<PTWInfo<W, T> > res_vec; res_vec.clear();
            int t = 5; while(t--)tree->get_suffix_info("1", res_vec, 3);
            sleep(0.1);
            break;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
template<class W, class T>
void * query_thread(void *args){
    ConcurrentTree<W, T> * tree = (ConcurrentTree<W, T> *)args;
    while(true){
        try{
            std::vector<PTWInfo<W, T> > res_vec; res_vec.clear();
            tree->get_suffix_info("1", res_vec, 3);
            tree->lock_query();
            for(uint32_t i=0; i <res_vec.size(); i++){
                cout<<"word="   << res_vec[i].word   <<endl;
                cout<<"info="   << res_vec[i].info->a<<endl;
                cout<<"weight=" <<*res_vec[i].weight <<endl;
                cout<<endl;
            }
            cout<<tree->get_query_count()<<" mic_second="<<get_micron_second()<<" datetime="<<getTodayDate()<<endl;
            tree->unlock_query();
            sleep(0.1);
        } catch(const char * e){
            cout<<e<<endl;
            throw;
        }
        break;
    }
}

void test_concurrentTree_insert(){
    ConcurrentTree<int, Info> * tree = new ConcurrentTree<int, Info>();

    pthread_t threads[11];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    const int ret0 = pthread_create(&threads[0], 0, update_thread<int, Info>, tree);
    const int ret1 = pthread_create(&threads[1], 0, update_thread<int, Info>, tree);
    const int ret2 = pthread_create(&threads[2], 0, update_thread<int, Info>, tree);
    const int ret3 = pthread_create(&threads[3], 0, update_thread<int, Info>, tree);

    const int ret4 = pthread_create(&threads[4], 0, query_thread<int, Info>, tree);
    const int ret5 = pthread_create(&threads[5], 0, query_thread<int, Info>, tree);
    const int ret6 = pthread_create(&threads[6], 0, query_thread<int, Info>, tree);
    const int ret7 = pthread_create(&threads[7], 0, query_thread<int, Info>, tree);

    const int ret8 = pthread_create(&threads[8], 0, delete_thread<int, Info>, tree);
    const int ret9 = pthread_create(&threads[9], 0, delete_thread<int, Info>, tree);
    const int ret10= pthread_create(&threads[10],0, delete_thread<int, Info>, tree);
    const int ret11= pthread_create(&threads[11],0, delete_thread<int, Info>, tree);

    void *status;
    pthread_attr_destroy(&attr);
    for(int i = 0; i < 12; i++){
        int rec = pthread_join(threads[i], &status);
        if(rec){
             cout << "Error:unable to join," << rec << endl;
             exit(-1);
        }
        cout << "Main: completed thread id :" << i << "  exiting with status :" << status << endl;
    }
    delete tree;
}
void test_replace(){
    ConcurrentReplacer replacer;
    replacer.insert("wulangzhou", "p_lzhouwu");
    cout<<replacer.replace("wulangzhou哈哈哈")<<endl;
}

void * update_replace_thread(void *args){
    ConcurrentReplacer * replacer = (ConcurrentReplacer *)args;
    try{
        for(int i = 0; i < 10000; i++){
            replacer->insert(toString(i).c_str(), toString(i+1).c_str(), true);
            sleep(1);
            cout<<"--------------------------------"<<endl;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
void * delete_replace_thread(void *args){
    ConcurrentReplacer * replacer = (ConcurrentReplacer *)args;
    try{
        for(int i = 0; i < 10000; i++){
            replacer->remove(toString(i).c_str());
            sleep(1);
            cout<<"+++++++++++++++++++++++++++++++++"<<endl;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
void * query_replace_thread(void *args){
    ConcurrentReplacer * replacer = (ConcurrentReplacer *)args;
    while(true){
        string content = toString((rand() % 10000000) + 1) + toString((rand() % 10000000) + 1);
        string res_str = replacer->replace(content.c_str());

        std::vector<std::pair<std::string, std::string> > res; //res.clear();
        replacer->get_replace_info(content.c_str(), res);

        replacer->lock_query();
        cout<<content<<endl;
        cout<<res_str<<endl;
        for(int i = 0; i < res.size(); i++){
            cout<<res[i].first<<" "<<res[i].second<<",";
        }
        cout<<endl;
        puts("");
        replacer->unlock_query();
        sleep(0.1);
    }
}
void test_concurrent_replace(){
    ConcurrentReplacer *replacer = new ConcurrentReplacer();

    pthread_t threads[11];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    const int ret0 = pthread_create(&threads[0], 0, update_replace_thread, replacer);
    const int ret1 = pthread_create(&threads[1], 0, update_replace_thread, replacer);
    const int ret2 = pthread_create(&threads[2], 0, update_replace_thread, replacer);
    const int ret3 = pthread_create(&threads[3], 0, update_replace_thread, replacer);

    const int ret8 = pthread_create(&threads[8], 0, delete_replace_thread, replacer);
    const int ret9 = pthread_create(&threads[9], 0, delete_replace_thread, replacer);
    const int ret10= pthread_create(&threads[10],0, delete_replace_thread, replacer);
    const int ret11= pthread_create(&threads[11],0, delete_replace_thread, replacer);

    const int ret4 = pthread_create(&threads[4], 0, query_replace_thread, replacer);
    const int ret5 = pthread_create(&threads[5], 0, query_replace_thread, replacer);
    const int ret6 = pthread_create(&threads[6], 0, query_replace_thread, replacer);
    const int ret7 = pthread_create(&threads[7], 0, query_replace_thread, replacer);

    void *status; pthread_attr_destroy(&attr);
    for(int i = 0; i < 11; i++){
        int rec = pthread_join(threads[i], &status);
        if(rec){
             cout << "Error:unable to join," << rec << endl;
             exit(-1);
        }
        cout << "Main: completed thread id :" << i << "  exiting with status :" << status << endl;
    }

}
void * update_map_thread(void *args){
    ConcurrentMap<string, string> * map = (ConcurrentMap<string, string> *)args;
    try{
        for(int i = 0; i < 10000; i++){
            map->insert(toString(i), toString(i+2));
            sleep(1);
            cout<<"--------------------------------"<<endl;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
void * delete_map_thread(void *args){
    ConcurrentMap<string, string> * map = (ConcurrentMap<string, string> *)args;
    try{
        for(int i = 0; i < 10000; i++){
            map->erase(toString(i));
            sleep(1);
            cout<<"+++++++++++++++++++++++++++++++++"<<endl;
        }
    }catch(const char *e){
        cout<<e<<endl;
        throw;
    }
}
void * query_map_thread(void *args){
    ConcurrentMap<string, string> * map = (ConcurrentMap<string, string> *)args;
    while(true){
        string key = toString(rand() % 100);

        string val = "";
        bool is_find = map->find(key, val);

        map->lock_query();

        if(is_find)cout<<"key="<<key<<",val="<<val<<endl;

        map->unlock_query();
        sleep(0.1);
    }
}
void test_concurrent_map(){
    ConcurrentMap<string, string> *map = new ConcurrentMap<string, string>();

    pthread_t threads[11];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    const int ret0 = pthread_create(&threads[0], 0, update_map_thread, map);
    const int ret1 = pthread_create(&threads[1], 0, update_map_thread, map);
    const int ret2 = pthread_create(&threads[2], 0, update_map_thread, map);
    const int ret3 = pthread_create(&threads[3], 0, update_map_thread, map);

    const int ret8 = pthread_create(&threads[8], 0, delete_map_thread, map);
    const int ret9 = pthread_create(&threads[9], 0, delete_map_thread, map);
    const int ret10= pthread_create(&threads[10],0, delete_map_thread, map);
    const int ret11= pthread_create(&threads[11],0, delete_map_thread, map);

    const int ret4 = pthread_create(&threads[4], 0, query_map_thread, map);
    const int ret5 = pthread_create(&threads[5], 0, query_map_thread, map);
    const int ret6 = pthread_create(&threads[6], 0, query_map_thread, map);
    const int ret7 = pthread_create(&threads[7], 0, query_map_thread, map);

    void *status; pthread_attr_destroy(&attr);
    for(int i = 0; i < 11; i++){
        int rec = pthread_join(threads[i], &status);
        if(rec){
             cout << "Error:unable to join," << rec << endl;
             exit(-1);
        }
        cout << "Main: completed thread id :" << i << "  exiting with status :" << status << endl;
    }

}
void normal_test1(){
    Tree<string, string> *tree = new Tree<string, string>();
    tree->insert("", "1", "1");
    vector<PTInfo<string> > vec; vec.clear();
    tree->cut_max("吴浪舟wulangzhou", vec);
    for(int i = 0; i < vec.size(); i++){
        cout<<vec[i].word<<endl;
        cout<<vec[i].info<<endl;
    }
    cout<<endl;

    vector<PTWInfo<string, string> > vec2; vec2.clear();
    tree->get_suffix_info("", vec2, 5);
    for(int i = 0; i < vec2.size(); i++){
        cout<<vec2[i].word<<endl;
        cout<<*vec2[i].info<<endl;
    }
}


//g++ -lpthread test.cpp tree.h utils/StrUtils.o -o a.out -I./ -I./utils/
//ps -ef |grep a.out|grep -v grep|awk -F ' ' '{print $2}'|xargs htop -p
int main(){
    //test_son_memory_release();
    //test_arr2map();
    //test_strWeight_suffix();
    //while(true) test_get_suffix_info();
    //test_cut_max();
    test_accuracy_get_suffix_info();
    //test_struct_info();
    //while(true) test_concurrentTree_insert();
    //while(true)test_replace();
    //test_concurrent_replace();
    //normal_test1();
    //test_concurrent_map();
    return 0;
}
