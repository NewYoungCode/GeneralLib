#pragma once
#include "General.h"
class ConfigIni {
private:
	size_t buffSize = MAX_PATH;
	std::string filename;
	std::string section;
	DWORD GetValue(const std::string &section, const std::string &key, const std::string &defaultValue, const std::string &filename, std::string&outResult)const;
	bool SetValue(const std::string & section, const std::string & key, const std::string & Value, const std::string & absoluteFilename)const;
public:
	ConfigIni();
	//FileName //一定要绝对路径
	ConfigIni(const std::string& filename, const std::string &defaultSection = "setting", bool create = true, size_t buffSize = MAX_PATH);
	void SetDefaultSection(const std::string section);
	bool DeleteKey(const std::string &key, const std::string &section = "");
	//读取ini中的字符
	std::string ReadString(const std::string &key, const std::string& defaultValue = "", const std::string &section = "") const;
	//读取ini中的数字
	float ReadFloat(const std::string &key, float defaultValue = 0, const std::string &section = "") const;
	//读取ini中的int数字
	int ReadInt(const std::string &key, int defaultValue = 0, const std::string &section = "") const;
	//写入ini
	bool WriteValue(const std::string &key, const std::string &value, const std::string &section = "")const;
	std::vector<std::string> GetSections();
	void DeleteSection(const std::string&section);
};
