#ifndef __wlz_TimeUtils__H_
#define __wlz_TimeUtils__H_

#include "iostream"
#include "string.h"
#include "stdint.h"

namespace UBase{

namespace Utils{

	std::string getTodayDate(int add_days=0, std::string format="%Y-%m-%d %H:%M:%S");

	uint64_t get_micron_second();
}

};
#endif
