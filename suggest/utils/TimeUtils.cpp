#include "TimeUtils.h"
#include "time.h"
#include "sys/time.h"

namespace UBase{
/*
*/
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

};
