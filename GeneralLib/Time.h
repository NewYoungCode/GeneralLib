#pragma once
#include "General.h"
#include <time.h>
namespace Time {
	inline std::string ToString(time_t time_);
	time_t FileTimeToTime(const FILETIME &ft);
	namespace Now {
		std::string ToString(const std::string &format = "yyyy-MM-dd hh:mm:ss");
		template<class T>
		//ͳ�ƺ�����ʱʱ��
		inline time_t StopWatch(const T&_func) {
			auto beg_t = std::chrono::system_clock::now();    //��ʼʱ��
			_func();
			auto end_t = std::chrono::system_clock::now();    //����ʱ��
			std::chrono::duration<double> diff = end_t - beg_t;//ʱ����s
			time_t total = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//ʱ����ms
			printf("Elapsed :%d ms\n", total);
			return total;
		}
	}
};