#ifndef __wlz_StrUtils__H_
#define __wlz_StrUtils__H_

# include <vector>
# include <string>
# include<stdint.h>
# include<iostream>

namespace UBase{

namespace Utils{

	bool Utf8ToU32(const std::string &content, std::vector<uint32_t> & vec, bool append=false);

	void U32ToUtf8(const uint32_t &ui, std::string &res, bool append=false);

	void U32ToUtf8(std::vector<uint32_t>::iterator begin,
				   std::vector<uint32_t>::iterator end,
				   std::string & content,
				   bool append=false);

	void PrintUnicode(const uint32_t &ui);

	uint32_t getU32len(const char *content);

	void PrintUString(const std::vector<uint32_t> &vec);

	char * get_random_str(int);

	std::string toString(int i);

}

}
#endif
