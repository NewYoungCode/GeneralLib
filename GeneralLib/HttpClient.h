#pragma once
#include "General.h"
#include <WinInet.h>
#include <Windows.h>
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"ws2_32.lib")
class HttpClient
{
public:
	int Get(const std::string&url, std::string outRespone);
};

