#include "JsonCpp.h"
#if USEJSONCPP
#ifdef  _DEBUG
#pragma comment (lib,"json_libd.lib")
#else
#pragma comment (lib,"json_lib.lib")
#endif
namespace Json {
	bool parse(const std::string&jsonStr, Json::Value&outValue) {
		Json::Reader rd;
		return rd.parse(jsonStr, outValue);
	}
};
#endif