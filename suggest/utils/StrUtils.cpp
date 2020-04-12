#include <StrUtils.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream> 
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>


bool Utf8ToU32(const std::string& str, std::vector<uint32_t>& vec, bool append) {
  uint32_t tmp;
  if(!append)vec.clear();
  for(size_t i = 0; i < str.size();) {
    if(!(str[i] & 0x80)) { // 0xxxxxxx
      // 7bit, total 7bit
      tmp = (uint8_t)(str[i]) & 0x7f;
      i++;
    } else if ((uint8_t)str[i] <= 0xdf && i + 1 < str.size()) { // 110xxxxxx
      // 5bit, total 5bit
      tmp = (uint8_t)(str[i]) & 0x1f;

      // 6bit, total 11bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;
      i += 2;
    } else if((uint8_t)str[i] <= 0xef && i + 2 < str.size()) { // 1110xxxxxx
      // 4bit, total 4bit
      tmp = (uint8_t)(str[i]) & 0x0f;

      // 6bit, total 10bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;

      // 6bit, total 16bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+2]) & 0x3f;

      i += 3;
    } else if((uint8_t)str[i] <= 0xf7 && i + 3 < str.size()) { // 11110xxxx
      // 3bit, total 3bit
      tmp = (uint8_t)(str[i]) & 0x07;

      // 6bit, total 9bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+1]) & 0x3f;

      // 6bit, total 15bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+2]) & 0x3f;

      // 6bit, total 21bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i+3]) & 0x3f;

      i += 4;
    } else {
      return false;
    }
    vec.push_back(tmp);
  }
  return true;
}


void U32ToUtf8(const uint32_t &ui, std::string& res, bool append) {
  if(!append) res.clear();
  if(ui <= 0x7f) {
    res += char(ui);
  } else if(ui <= 0x7ff) {
    res += char(((ui >> 6) & 0x1f) | 0xc0);
    res += char((ui & 0x3f) | 0x80);
  } else if(ui <= 0xffff) {
    res += char(((ui >> 12) & 0x0f) | 0xe0);
    res += char(((ui >> 6) & 0x3f) | 0x80);
    res += char((ui & 0x3f) | 0x80);
  } else {
    res += char(((ui >> 18) & 0x03) | 0xf0);
    res += char(((ui >> 12) & 0x3f) | 0x80);
    res += char(((ui >> 6) & 0x3f) | 0x80);
    res += char((ui & 0x3f) | 0x80);
  }
}

void U32ToUtf8(std::vector<uint32_t>::iterator begin,
               std::vector<uint32_t>::iterator end,
               std::string& res,
               bool append) {
    if(!append) res.clear();
    for(std::vector<uint32_t>::iterator it = begin; it != end; it++){
        U32ToUtf8(*it, res, true);
    }
}

uint32_t getU32len(const char * content){
    std::vector<uint32_t> vec;
    if(Utf8ToU32(content, vec, false)){
        return vec.size();
    }
    return 0;
}

void PrintUnicode(const uint32_t &ui) {
    std::string res;
    U32ToUtf8(ui, res, false);
    std::cout<<res<<std::endl;
}


void PrintUString(const std::vector<uint32_t> &vec) {
    std::string res = "";
    for(int i = 0; i < vec.size(); i++){
        U32ToUtf8(vec[i], res, true);
    }
    std::cout<<res<<std::endl;
}

char * get_random_str(int len){
    char * p = (char *) malloc(sizeof(char *) * (len + 1));
    for(int i = 0; i < len; i++) {
        int s = rand() % 2;                     //随机使s为1或0，为1就是大写，为0就是小写 
        if(s == 1)                        //如果s=1 
            p[i] = rand() % ('Z'-'A'+1) + 'A';       //将x赋为大写字母的ascii码 
        else 
            p[i] = rand() % ('z'-'a'+1) + 'a';       //如果s=0，x赋为小写字母的ascii码 
    }
    p[len] = '\0';
    return p;
}

std::string toString(int i){
    std::stringstream ss; ss<<i; 
    return ss.str();
}

std::string getTodayDate(int add_days, std::string format){
    time_t timep;
    time (&timep);
    char tmp[64];
    timep = timep + 3600 * 24 * add_days;
    strftime(tmp, sizeof(tmp), format.c_str(), localtime(&timep));
    return tmp;
}

uint64_t get_micron_second(){
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);
    return tv.tv_sec*1000000 +tv.tv_usec;
}
