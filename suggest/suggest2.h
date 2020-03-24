# include <map>
# include <pthread.h>
# include <string.h>
# include <unistd.h>
# include <stdint.h>

template<class T, class W> class Node{
    int32_t in_num;
    bool is_end;
    W weight;
    T info;
};

template<class W, class T, uint32_t son_num=1> class NodeArr: public Node<W, T>{
public:
    NodeArr(W weight){
        this->weight = weight;
        this->is_end = false;
        this->in_num = 0;
        this->son_word = NULL;
        this->son_next = NULL;
        this->son_max = NULL;
    }
    void init_son(){
        this->son_word = (uint32_t *)malloc(sizeof(uint32_t) * son_num);
        this->son_max = (int32_t *)malloc(sizeof(int32_t) * son_num);
        this->son_next = new pair<void *, uint8_t>[son_num];
    }
public:
    int32_t *son_max;
    uint32_t *son_word;
    pair<void *, uint8_t> *son_next;
};

template<class W, class T, uint32_t top_num=25> class NodeMap: public Node<W, T>{
public:
   NodeMap(W weight){
        this->weight = weight;
        this->is_end = false;
        this->in_num = 0;
        this->son_next_map = NULL;
        this->son_top_vec = NULL;
    }
    void init_son(){
        this->son_next_map = new map<uint32_t, pair<void *, uint8_t> >;
        this->son_top_vec = new vector<pair<uint32_t, uint8_t> >;
    }
public:
    map<uint32_t, pair<void *, uint8_t> > *son_next_map;
    vector<pair<uint32_t, uint8_t> > *son_top_vec;
};

template<class T> struct ResInfo{
    string phrase;
    uint32_t position;
    uint32_t phrase_len;
    int32_t weight;
    T info;
};

template<class T, class W, uint32_t top_num=25> class Tree{
    pthread_mutex_t *up_lock;
    pthread_mutex_t *up_lock2;
    pthread_cond_t *up_cond;
    bool in_update;
    int32_t query_count;
    pair<void *, uint8_t> root;
    vector<uint8_t> resize_offset_vec;
    Tree(){
        resize_offset_vec.clear();
        resize_offset_vec.push_back(1);
        resize_offset_vec.push_back(3);
        resize_offset_vec.push_back(6);

        up_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        up_lock2 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        up_cond = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

        pthread_mutex_init(up_lock, NULL);
        pthread_cond_init(up_cond, NULL);

        root->first = new NodeMap<T, W, top_num>;
        root->second = 0;

        query_count = 0;
        in_update = false;
    }
};
