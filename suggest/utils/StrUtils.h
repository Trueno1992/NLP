#include <vector>
#include <string>
# include<stdint.h>
# include<iostream>


bool Utf8ToU32(const std::string &, std::vector<uint32_t> &);

void U32ToUtf8(const uint32_t &, std::string &);

void PrintUnicode(const uint32_t &);

void PrintUString(const std::vector<uint32_t> &);

char * get_random_str(int);
