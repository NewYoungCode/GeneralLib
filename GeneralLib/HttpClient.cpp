#include "HttpClient.h"

int HttpClient::Get(const std::string & url, std::string outRespone)
{
	outRespone.clear();

	HINTERNET hintInternetOpen = InternetOpen("Testing", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hintInternetOpen)
	{
		InternetCloseHandle(hintInternetOpen);
		return 1;
	}
	HINTERNET hintInternetOpenUrl = InternetOpenUrl(hintInternetOpen, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (!hintInternetOpenUrl)
	{
		InternetCloseHandle(hintInternetOpen);
		InternetCloseHandle(hintInternetOpenUrl);
		return 1;
	}

	while (true)
	{
		DWORD dwByteRead = 0;
		char szDownBuffer[256]{ 0 };
		auto ret = InternetReadFile(hintInternetOpenUrl, szDownBuffer, 256, &dwByteRead);
		if (!dwByteRead)
		{
			break;
		}
		outRespone.append(szDownBuffer, dwByteRead);
	}

	InternetCloseHandle(hintInternetOpen);
	InternetCloseHandle(hintInternetOpenUrl);

	File::Delete("d:\\aa.json");
	std::ofstream ofs("d:\\aa.json",std::ios::app|std::ios::binary);
	ofs.write(outRespone.c_str(), outRespone.size());
	ofs.flush();
	ofs.close();

}
