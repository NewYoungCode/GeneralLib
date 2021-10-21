#pragma once
#include "General.h"
#ifdef _DEBUG
#pragma comment(lib,"GeneralLibD.lib")
#else
#pragma comment(lib,"GeneralLib.lib")
#endif

namespace comm {
	inline void Log(const std::string &text) {
		OutputDebugStringA(text.c_str());
		OutputDebugStringA(("\n"));
		std::string appFilename = Path::StartFileName();
		std::string appName = Path::GetFileNameWithoutExtension(appFilename);
		std::string dir = Path::GetDirectoryName(appFilename) + "/" + appName + "_Log";
		std::string logFilename = dir + "/" + Time::Now::ToString("yyyy-MM-dd") + ".log";
		Path::Create(dir);
		std::string logText = Time::Now::ToString("hh:mm:ss ") + text;
		std::ofstream file(logFilename.c_str(), std::ios::app);
		printf("%s\n", logText.c_str());
		file << logText.c_str() << std::endl;
		file.flush();
		file.close();
		//删除多余的日志 只保留当天的日志
		auto today = Time::Now::ToString("yyyy-MM-dd");
		auto files = Path::SearchFiles(dir, "*.log");
		for (auto&it : files) {
			auto t = Path::GetFileNameWithoutExtension(it);
			if (t.size() < 10 || t.substr(0, 10) != today) {
				File::Delete(it);
			}
		}
	}
	inline void Log(const std::wstring &wtext) {
		Log(Text::UnicodeToANSI(wtext));
	}
}