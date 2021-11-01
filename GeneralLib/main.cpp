#include "General.h"
#include "ConfigIni.h"
#include "HttpClient.h"
int main() {


	HttpClient hc;

	std::string sp;
	hc.Get("https://m.kugou.com/?json=true", sp);

	SafeConfigIni::EnCode("D:\\config.ini", "D:\\config.ini2");

	SafeConfigIni ini("D:\\config.ini2");

	auto zip=ini.ReadString("zip","","setting");

	return 0;
}