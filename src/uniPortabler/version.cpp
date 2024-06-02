import fs_common;
#include <filesystem>
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <vector>


void fs_core_init(const std::wstring& config_path);
using namespace std;
namespace fs = filesystem;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);


		fs_core_init(findConfigPath());
		MessageBeep(MB_ICONINFORMATION);

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		//RemoveHook();
		break;
	}
	}
	return TRUE; // 返回TRUE表示初始化成功，返回FALSE可能导致DLL加载失败
}
