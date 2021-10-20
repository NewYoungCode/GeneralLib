#include "WinTool.h"

namespace WinTool {

	// ��ȡϵͳ��Ϣ
	 void SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
	{
		if (NULL == lpSystemInfo)
		{
			return;
		}
		typedef VOID(WINAPI *FuncGetSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
		FuncGetSystemInfo funcGetNativeSystemInfo = (FuncGetSystemInfo)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetNativeSystemInfo");
		// ����ʹ�� "GetNativeSystemInfo" ��������ȡϵͳ��Ϣ
		// ���� "GetSystemInfo" ����ϵͳλ������������
		if (NULL != funcGetNativeSystemInfo)
		{
			funcGetNativeSystemInfo(lpSystemInfo);
		}
		else
		{
			GetSystemInfo(lpSystemInfo);
		}
	}

	// ��ȡ����ϵͳλ��
	 int GetSystemBits()
	{
		SYSTEM_INFO si;
		SafeGetNativeSystemInfo(&si);
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return 64;
		}
		return 32;
	}



	 DWORD GetCurrentProcessId() {
		return ::getpid();
	}
	 HWND FindMainWindow(DWORD processId)
	{
		handle_data data;
		data.processId = processId;
		data.best_handle = 0;
		EnumWindows(EnumWindowsCallback, (LPARAM)&data);
		return data.best_handle;
	}
	 BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		HWND hwnd = ::GetWindow(handle, GW_OWNER);
		data.best_handle = handle;
		return 0;

		//unsigned long processId = 0;
		//::GetWindowThreadProcessId(handle, &processId);
		//if (data.processId != processId || !IsMainWindow(handle)) {
		//	return TRUE;
		//}
		//data.best_handle = handle;
		return FALSE;
	}

	 std::vector<PROCESSENTRY32> FindProcessInfo(const std::string& _proccname) {

		std::vector<PROCESSENTRY32> infos;
		HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		for (auto status = ::Process32First(hSnapshot, &pe); status != FALSE; status = ::Process32Next(hSnapshot, &pe)) {
			pe.dwSize = sizeof(PROCESSENTRY32);
#ifdef UNICODE
			std::string item = Text::UnicodeToANSI(pe.szExeFile);
#else
			std::string item = pe.szExeFile;
#endif
			//�����������Ʋ�ѯ����
			if (_proccname.empty()) {
				infos.push_back(pe);
			}
			else {
				if (item == _proccname) {
					infos.push_back(pe);
				}
			}
			//printf("%s %d\n", item.data(),pe.th32processId);
		}
		CloseHandle(hSnapshot);
		return infos;
	}
	 std::vector<DWORD> FindProcessId(const std::string& _proccname)
	{
		std::vector<DWORD> processIds;
		auto list = FindProcessInfo(_proccname);
		for (auto &it : list) {
			processIds.push_back(it.th32ProcessID);
		}
		return processIds;
	}

	 HANDLE OpenProcess(const std::string& _proccname) {
		std::vector<DWORD> processIds;
		auto list = FindProcessInfo(_proccname);
		if (list.size() > 0) {
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, list.at(0).th32ProcessID);
			return hProcess;
		}
		return NULL;
	}

	 	bool Is86BitPorcess(DWORD processId) {

		return !Is64BitPorcess(processId);
	}

	 	bool Is64BitPorcess(DWORD processId)
	{
		BOOL bIsWow64 = FALSE;
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
		if (hProcess)
		{
			typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
			LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), TEXT("IsWow64Process"));
			if (NULL != fnIsWow64Process)
			{
				fnIsWow64Process(hProcess, &bIsWow64);
			}
		}
		CloseHandle(hProcess);
		return !bIsWow64;
	}

	 std::string FindProcessFilename(DWORD processId)
	{
		char buf[MAX_PATH]{ 0 };
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		DWORD result = ::GetModuleFileNameExA(hProcess, NULL, buf, sizeof(buf) - 1);
		CloseHandle(hProcess);
		return buf;
	}
	 int CloseProcess(const std::vector<DWORD>& processIds) {
		size_t count = 0;
		for (auto item : processIds) {
			count += CloseProcess(item);
		}
		return count;
	}
	 bool CloseProcess(DWORD processId)
	{
		HANDLE bExitCode = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE
			| PROCESS_ALL_ACCESS, FALSE, processId);
		if (bExitCode)
		{
			BOOL bFlag = ::TerminateProcess(bExitCode, 0);
			CloseHandle(bExitCode);
			return true;
		}
		return false;
	}
	 bool GetAutoBootStatus(const std::string& filename) {
		std::string bootstart = filename.empty() ? Path::StartFileName() : filename;
		std::string appName = Path::GetFileNameWithoutExtension(bootstart);
		bool bResult = false;
		HKEY subKey;
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), NULL, KEY_ALL_ACCESS, &subKey))
		{
			return bResult;
		}
		//3���ж�ע������Ƿ��Ѿ�����
		char strDir[MAX_PATH]{ 0 };
		DWORD nLength = MAX_PATH;
		LSTATUS status = RegGetValue(subKey, NULL, AUTOTEXT(appName), REG_SZ, NULL, strDir, &nLength);
		if (status != ERROR_SUCCESS) {
			if (bootstart == strDir) {
				bResult = true;
			}
		}
		::RegCloseKey(subKey);
		return bResult;
	}
	 bool SetAutoBoot(const std::string& filename, bool bStatus)
	{
		//�����Ҫ���ó������������Ѿ����������ͷ���true
		if (bStatus && GetAutoBootStatus(filename)) {
			return true;
		}
		if (!bStatus && !GetAutoBootStatus(filename)) {
			return true;
		}
		std::string bootstart = filename.empty() ? Path::StartFileName() : filename;
		std::string appName = Path::GetFileNameWithoutExtension(bootstart);
		bool bResult = false;
		HKEY subKey;
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), NULL, KEY_ALL_ACCESS, &subKey))
		{
			return bResult;
		}
		if (bStatus)
		{

			if (ERROR_SUCCESS == ::RegSetValueEx(subKey, AUTOTEXT(appName), NULL, REG_SZ, (PBYTE)bootstart.c_str(), bootstart.size()))
			{
				bResult = true;
			}
		}
		else
		{
			if (ERROR_SUCCESS == ::RegDeleteValue(subKey, AUTOTEXT(appName)))
			{
				bResult = true;
			}
		}
		::RegCloseKey(subKey);
		return bResult;
	}
	 BOOL EnablePrivilege(HANDLE process)
	{
		// �õ����ƾ��
		HANDLE hToken = NULL;
		bool bResult = false;
		if (OpenProcessToken(process ? process : ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY | TOKEN_READ, &hToken)) {
			// �õ���Ȩֵ
			LUID luid;
			if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
				// �������ƾ��Ȩ��
				TOKEN_PRIVILEGES tp = {};
				tp.PrivilegeCount = 1;
				tp.Privileges[0].Luid = luid;
				tp.Privileges[0].Attributes = 1 ? SE_PRIVILEGE_ENABLED : 0;
				if (AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL)) {
					bResult = true;
				}
			}
		}
		// �ر����ƾ��
		CloseHandle(hToken);
		return bResult;
	}
	 bool CreateDesktopLnk(const std::string &pragmaFilename, const std::string &LnkName, const std::string& cmdline, const std::string& iconFilename) {
		HRESULT hr = CoInitialize(NULL);
		bool bResult = false;
		if (SUCCEEDED(hr))
		{
			IShellLink *pShellLink;
			hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
			if (SUCCEEDED(hr))
			{
				pShellLink->SetPath(AUTOTEXT(pragmaFilename));
				pShellLink->SetWorkingDirectory(AUTOTEXT(Path::GetDirectoryName(pragmaFilename)));
				pShellLink->SetArguments(AUTOTEXT(cmdline));
				if (!iconFilename.empty())
				{
					pShellLink->SetIconLocation(AUTOTEXT(iconFilename), 0);
				}
				IPersistFile* pPersistFile;
				hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
				if (SUCCEEDED(hr))
				{
					char buf[MAX_PATH]{ 0 };
					SHGetSpecialFolderPathA(0, buf, CSIDL_DESKTOPDIRECTORY, 0);//��ȡ��ǰ�û�����·��
					std::string userDesktop(buf);
					if (!LnkName.empty()) {
						userDesktop += "\\" + LnkName + ".lnk";
					}
					else {
						userDesktop += "\\" + Path::GetFileNameWithoutExtension(pragmaFilename) + ".lnk";
					}
					//���ÿ�ݷ�ʽ��ַ
					File::Delete(userDesktop);//ɾ���ɵ�
					hr = pPersistFile->Save(Text::ANSIToUniCode(userDesktop).c_str(), FALSE);
					if (SUCCEEDED(hr))
					{
						bResult = true;
					}
					pPersistFile->Release();
				}
				pShellLink->Release();
			}
		}
		CoUninitialize();
		return bResult;
	}
};