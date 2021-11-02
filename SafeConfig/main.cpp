// SafeConfig.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <os.h>
#include <ConfigIni.h>
int main(int count, char**strs)
{
	if (count < 2) return 0;
	std::string fullFilename = strs[1];
	std::string filename = Path::GetFileNameWithoutExtension(fullFilename);
#if 1
	std::string out = Path::GetDirectoryName(fullFilename) + "\\" + filename;
	SafeConfigIni::EnCode(fullFilename, out);
	::MessageBox(0, "加密成功", "", 0);
#else
	std::string out = Path::GetDirectoryName(fullFilename) + "\\" + filename + ".ini";
	SafeConfigIni::DeCode(fullFilename, out);
	::MessageBox(0, "解密成功", "", 0);
#endif
	return 0;
}

