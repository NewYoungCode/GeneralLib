#include "JsonCpp.h"
#if USEJSONCPP
JObject::JObject(const Json::Value&right) {
	Json::Value temp(right);
	swap(temp);
}
JObject& JObject:: operator = (const Json::Value &other)
{
	Value temp(other);
	swap(temp);
	return *this;
}
JObject::JObject(const std::string&jsonStr) {
	rd.parse(jsonStr, *this);
}
#endif