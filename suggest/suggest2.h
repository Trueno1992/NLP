# include <map>
# include <vector>
# include <pthread.h>
# include <string.h>
# include <stdint.h> // uint32_t
# include <utility>  // 包含pair

template<class T> struct ResInfo{
    std::string phrase;
    uint32_t position;
    uint32_t phrase_len;
    int32_t weight;
    T info;
};

template<class T, class W> class Node{
public:
    int32_t in_num;
    bool is_end;
    W weight;
    T info;
};

template<class W, class T, uint32_t son_num=1> class NodeArr: public Node<W, T>{
public:
    NodeArr(){
        this->is_end = false;
        this->in_num = 0;
        this->son_word = NULL;
        this->son_next = NULL;
        this->son_max = NULL;
    }
    void init_son(){
        this->son_word = (uint32_t *)malloc(sizeof(uint32_t *) * son_num);
        this->son_max = (W *)malloc(sizeof(W *) * son_num);
        this->son_next = new std::pair<void *, uint8_t> [son_num];
    }
    void resize(uint32_t new_son_num){
        W * new_son_word = (W *)malloc(sizeof(W *) * new_son_num);
        uint32_t * new_son_max = (int32_t *)malloc(sizeof(int32_t *) * new_son_num);
        std::pair<void *, uint8_t> * new_son_next = new std::pair<void *, uint8_t>[new_son_num];
        for(uint32_t i = 0; i < this->son_num; ++i){
            new_son_max[i] = this->son_max[i];
            new_son_word[i] = this->son_word[i];
            new_son_next[i] = this->son_next[i];
        }
        free(this->son_max); this->son_max = new_son_max;
        free(this->son_word); this->son_word = new_son_word;
        delete new_son_next; this->son_next = new_son_next;
        this->son_num = new_son_num;
    }
    bool resize(std::vector<uint8_t> &off_size){
        for(uint32_t i = 0; i < off_size(); i++){
            if(off_size[i] > this->son_num){
                this->resize(off_size[i]);
                return true;
            }
        }
        return false;
    }
    void nodeArr_to_nodeMap(uint32_t);
public:
    W *son_max;
    uint32_t *son_word;
    std::pair<void *, uint8_t> *son_next;
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
        this->son_next_map = new std::map<uint32_t, std::pair<std::pair<void *, uint8_t> *, W> >;
        this->son_top_vec = new std::vector<std::pair<uint32_t, W> >;
    }
public:
    std::map<uint32_t, std::pair<std::pair<void *, uint8_t> *, W> > *son_next_map;
    std::vector<std::pair<uint32_t, W> > *son_top_vec;
};

template<class T, class W, uint32_t top_num=25> class Tree{
public:
    Tree(std::vector<uint8_t> &offset_vec=NULL){
        resize_offset_vec.clear();
        if(offset_vec != NULL && offset_vec.size() != 0){
            resize_offset_vec.push_back(offset_vec.begin(), offset_vec.end());
        }else{
            resize_offset_vec.push_back(1);
            resize_offset_vec.push_back(3);
            resize_offset_vec.push_back(6);
        }
        up_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        up_lock2 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
        up_cond = (pthread_cond_t  *)malloc(sizeof(pthread_cond_t)); 

        pthread_mutex_init(up_lock, NULL);
        pthread_cond_init(up_cond, NULL);

        root.first = new NodeMap<T, W, top_num>;
        root.second = 0;

        query_count = 0;
        in_update = false;
    }
    ~Tree(){
        free_root();
        free(up_lock);
        free(up_lock2);
        free(up_cond);
        resize_offset_vec.clear();
        delete root.first;
    }

public:
    pthread_mutex_t *up_lock;
    pthread_mutex_t *up_lock2;
    pthread_cond_t *up_cond;
    bool in_update;
    int32_t query_count;
    std::pair<void *, uint8_t> root;
    std::vector<uint8_t> resize_offset_vec;
};

template<class W, class T, uint32_t son_num=1> NodeMap<W, T, top_num> *NodeArr::nodeArr_to_nodeMap(uint32_t top_num){
    NodeMap<W, T, top_num> *nodemap = new NodeMap<W, T, top_num>;
    for(int i = 0; i <= this->son_num; i++){
        nodemap->son_next_map[this->son_word[i]] = this->son_next[i];
    }
}
