# include<iostream>
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<wchar.h>
# include<locale.h>
# include<map>
# include<vector>

using namespace std;


struct PhraseInfo{
    int index;
    const wchar_t * phrase;
    PhraseInfo * next;
};


struct Node{
    bool isEnd;
    int phraseLen;
    const wchar_t * phrase;
    map<wchar_t, Node *> *nextMap;
};


bool insert(Node * root, const wchar_t * word){
    const wchar_t * origin_word = word;
    Node * p = root;
    map<wchar_t, Node *>::iterator it;
    while( *word ){
        it = p->nextMap->find( *word );
        if( it != p->nextMap->end() ){
            p = it->second;
        }else{
            Node * tmp = (Node *) malloc (sizeof(Node));
            tmp->isEnd = false;
            tmp->nextMap = new map<wchar_t, Node *>;
            tmp->nextMap->clear();
            tmp->phraseLen = 0;
            (*(p->nextMap))[*word] = tmp;
            p = tmp;
        }
        word++;
    }

    if( p == root ) return true;
    if( p->isEnd ) return false;
    p->phrase = origin_word;
    p->phraseLen = wcslen(p->phrase);
    p->isEnd = true;
    return true;
}


PhraseInfo * getCutInfo(Node * root, const wchar_t * word){
    int idx = 0;
    int offerset = 1;
    int tmpIdx = 0;
    map<wchar_t, Node *>::iterator it;
    Node * tp = NULL;
    PhraseInfo * pre_PhraseInfo = NULL;
    PhraseInfo * start_PhraseInfo = NULL;
    PhraseInfo * tmpPhraseInfo = NULL;
    const wchar_t * tmpWord = NULL;
    while( *word ){
      tp = root;
      tmpPhraseInfo = new PhraseInfo();
      tmpPhraseInfo->phrase = NULL;
      offerset = 1;
      tmpIdx = idx;
      tmpWord = word;
      while( *tmpWord ){
          it = tp->nextMap->find( *tmpWord );
          if( it != tp->nextMap->end() ){
              tp = it->second;
          }else{
              if(tmpPhraseInfo->phrase == NULL){
                  wchar_t x[] = {*word, '\0'};
                  wchar_t *str = (wchar_t *)malloc(sizeof(wchar_t) * (sizeof(x) + 1));
                  wcscpy(str, x);
                  tmpPhraseInfo->phrase = str;
                  tmpPhraseInfo->index = idx;
                  tmpPhraseInfo->next = NULL;
                  insert(root, str);
              }
              break;
          }
          if( tp->isEnd ){
              tmpPhraseInfo->index = tmpIdx;
              tmpPhraseInfo->phrase = tp->phrase;
              offerset = tp->phraseLen;
          }
          tmpIdx++;
          tmpWord++;
      }
      if(tmpPhraseInfo->phrase == NULL){
          wchar_t x[] = {*word, '\0'};
          wchar_t *str = (wchar_t *)malloc(sizeof(wchar_t) * (sizeof(x) + 1));
          wcscpy(str, x);
          tmpPhraseInfo->phrase = str;
          tmpPhraseInfo->index = idx;
          tmpPhraseInfo->next = NULL;
          insert(root, str);
      }
      if( pre_PhraseInfo != NULL){
          pre_PhraseInfo->next = tmpPhraseInfo;
      }else{
          start_PhraseInfo = tmpPhraseInfo;
      }
      pre_PhraseInfo = tmpPhraseInfo;
      idx += offerset;
      word += offerset;
    }
    return start_PhraseInfo;
}


void free_CutInfo_list_prx(PhraseInfo * p){
    PhraseInfo * c = NULL;
    while(p != NULL){
        c = p->next;
        delete p;
        p = c;
    }
}


void free_CutInfo_list_prx2(PhraseInfo * p){
    if(p->next != NULL){
        free_CutInfo_list_prx2(p->next);
    }
    delete p;
}


extern "C"{
    Node * o_root;
    bool is_init = false;

    void init(){
        if( !is_init ){
            setlocale(LC_CTYPE, "zh_CN.utf8");
            o_root = new Node();
            o_root->isEnd = false;
            o_root->nextMap = new map<wchar_t, Node *>;
            o_root->nextMap->clear();
            is_init = true;
        }
    }

    wchar_t * get_wchar_str(const char * str){
        int len = strlen(str) + 1;
        wchar_t * wstr = (wchar_t*)malloc(len*sizeof(wchar_t));
        mbstowcs(wstr, str, len);
        return wstr;
    }

    bool insert_prx(const char * str){
        init();
        if (strlen(str) <= 0) return false;
        return insert(o_root, get_wchar_str(str));
    }

    PhraseInfo * getcutinfo_prx(const char * str){
        init();
        if (strlen(str) <= 0) return NULL;
        wchar_t * wstr = get_wchar_str(str);
        PhraseInfo * cutinfohead = getCutInfo(o_root, wstr);
        free(wstr);
        return cutinfohead;
    }

    void * free_CutInfo_list(PhraseInfo * p){
        free_CutInfo_list_prx(p);
    }
}


int main(){
    setlocale(LC_CTYPE, "zh_CN.utf8");
    Node * root = (Node *) malloc (sizeof(Node));
    root->isEnd = false;
    root->nextMap = new map<wchar_t, Node *>;
    root->nextMap->clear();
    insert(root, L"wulangzhou");
    insert(root, L"吴浪舟");
    insert(root, L"吴浪");
    insert(root, L"maxiaoyu");
    insert(root, L"and");
    insert(root, L"大家看");
    //const wchar_t * string1 = L"大家看，大家快看，快看";
    const wchar_t * string1 = L"吴浪舟andwulangzhou大家看大家看大家看大家看大家看";
    wprintf(L"%S\n", string1);
    /*
    int i = 100000000;
    while(i >= 0){
        PhraseInfo * start_PhraseInfo = getCutInfo(root, string1);
        free_CutInfo_list_prx(start_PhraseInfo);
        i -= 1;
        if(i % 1000000 == 0)
        wcout<<i<<endl;
    }
    wcout<<"fds"<<endl;
    */
    PhraseInfo * start_PhraseInfo = getCutInfo(root, string1);
    while(start_PhraseInfo != NULL){
        wprintf(L"%d, %d, %S\n", (*start_PhraseInfo).index, wcslen((*start_PhraseInfo).phrase), (*start_PhraseInfo).phrase);
        start_PhraseInfo = (*start_PhraseInfo).next;
    }
    return 0;
}
