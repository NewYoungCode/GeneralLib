#pragma once
#include "General.h"
#if USEJSONCPP
#include <json/json.h>
namespace Json {
	bool parse(const std::string&jsonStr, Json::Value&outValue);
};
#endif