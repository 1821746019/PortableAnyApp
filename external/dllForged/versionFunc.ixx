module;

#include <Windows.h>
export module versionFunc;

import std;
HMODULE loadSysDll(const std::string& sysDllName)
{
#ifdef _WIN64
	auto dll_path = R"(C:\Windows\System32\)" + sysDllName;
#else
	auto dll_path = R"(C:\Windows\SysWOW64\)" + sysDllName;
#endif

	auto ret = LoadLibraryA(dll_path.data());
	return ret;
}
static HMODULE hDll = loadSysDll("version.dll");

//	auto GetFileVersionInfoByHandle_bridge =
//		GetProcAddress(hDll, "GetFileVersionInfoByHandle");


//decltype(GetFileVersionInfoA)&& GetFileVersionInfoA_bridge = GetFileVersionInfoA;

BOOL APIENTRY GetFileVersionInfoA_bridge(LPCSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData)
{
	using Func = decltype(&GetFileVersionInfoA);
	static Func GetFileVersionInfoA = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoA");
	return GetFileVersionInfoA(lptstrFilename, dwHandle, dwLen, lpData);
}

// GetFileVersionInfoW
BOOL APIENTRY GetFileVersionInfoW_bridge(LPCWSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData)
{
	using Func = decltype(&GetFileVersionInfoW);
	static Func GetFileVersionInfoW = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoW");
	return GetFileVersionInfoW(lptstrFilename, dwHandle, dwLen, lpData);
}

// GetFileVersionInfoExA
BOOL APIENTRY GetFileVersionInfoExA_bridge(DWORD dwFlags, LPCSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData)
{
	using Func = decltype(&GetFileVersionInfoExA);
	static Func GetFileVersionInfoExA = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoExA");

	return GetFileVersionInfoExA(dwFlags, lptstrFilename, dwHandle, dwLen, lpData);

}

// GetFileVersionInfoExW
BOOL APIENTRY GetFileVersionInfoExW_bridge(DWORD dwFlags, LPCWSTR lptstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData)
{
	using Func = decltype(&GetFileVersionInfoExW);
	static Func GetFileVersionInfoExW = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoExW");
	return GetFileVersionInfoExW(dwFlags, lptstrFilename, dwHandle, dwLen, lpData);
}

// GetFileVersionInfoSizeA
DWORD APIENTRY GetFileVersionInfoSizeA_bridge(LPCSTR lptstrFilename,
	DWORD* lpdwHandle)
{
	using Func = decltype(&GetFileVersionInfoSizeA);
	static Func GetFileVersionInfoSizeA = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoSizeA");
	return GetFileVersionInfoSizeA(lptstrFilename, lpdwHandle);
}

// GetFileVersionInfoSizeW
DWORD APIENTRY GetFileVersionInfoSizeW_bridge(LPCWSTR lptstrFilename,
	DWORD* lpdwHandle)
{
	using Func = decltype(&GetFileVersionInfoSizeW);
	static Func GetFileVersionInfoSizeW = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoSizeW");
	return GetFileVersionInfoSizeW(lptstrFilename, lpdwHandle);
}

// GetFileVersionInfoSizeExA
DWORD APIENTRY GetFileVersionInfoSizeExA_bridge(DWORD dwFlags, LPCSTR lptstrFilename,
	DWORD* lpdwHandle)
{

	using Func = decltype(&GetFileVersionInfoSizeExA);
	static Func GetFileVersionInfoSizeExA = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoSizeExA");
	return GetFileVersionInfoSizeExA(dwFlags, lptstrFilename, lpdwHandle);
}

// GetFileVersionInfoSizeExW
DWORD APIENTRY GetFileVersionInfoSizeExW_bridge(DWORD dwFlags, LPCWSTR lptstrFilename,
	DWORD* lpdwHandle)
{
	using Func = decltype(&GetFileVersionInfoSizeExW);
	static Func GetFileVersionInfoSizeExW = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoSizeExW");
	return GetFileVersionInfoSizeExW(dwFlags, lptstrFilename, lpdwHandle);
}

// VerFindFileA
DWORD APIENTRY VerFindFileA_bridge(DWORD uFlags, LPCSTR szFileName,
	LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen,
	LPSTR szDestDir, PUINT lpuDestDirLen)
{
	using Func = decltype(&VerFindFileA);
	static Func VerFindFileA = (Func)GetProcAddress
		(hDll, "VerFindFileA");
	return VerFindFileA(uFlags,
		szFileName,
		szWinDir,
		szAppDir,
		szCurDir,
		lpuCurDirLen,
		szDestDir,
		lpuDestDirLen);
}

// VerFindFileW
DWORD APIENTRY VerFindFileW_bridge(DWORD uFlags, LPCWSTR szFileName,
	LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen,
	LPWSTR szDestDir, PUINT lpuDestDirLen)
{
	using Func = decltype(&VerFindFileW);
	static Func VerFindFileW = (Func)GetProcAddress
		(hDll, "VerFindFileW");
	return VerFindFileW(uFlags,
		szFileName,
		szWinDir,
		szAppDir,
		szCurDir,
		lpuCurDirLen,
		szDestDir,
		lpuDestDirLen);
}

// VerInstallFileA
DWORD APIENTRY VerInstallFileA_bridge(DWORD uFlags, LPCSTR szSrcFileName,
	LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir,
	LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen)
{
	using Func = decltype(&VerInstallFileA);
	static Func VerInstallFileA = (Func)GetProcAddress
		(hDll, "VerInstallFileA");
	return VerInstallFileA(uFlags,
		szSrcFileName,
		szDestFileName,
		szSrcDir,
		szDestDir,
		szCurDir,
		szTmpFile,
		lpuTmpFileLen);
}

// VerInstallFileW
DWORD APIENTRY VerInstallFileW_bridge(DWORD uFlags, LPCWSTR szSrcFileName,
	LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir,
	LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen)
{
	using Func = decltype(&VerInstallFileW);
	static Func VerInstallFileW = (Func)GetProcAddress
		(hDll, "VerInstallFileW");
	return VerInstallFileW(uFlags,
		szSrcFileName,
		szDestFileName,
		szSrcDir,
		szDestDir,
		szCurDir,
		szTmpFile,
		lpuTmpFileLen);
}

// VerLanguageNameA
DWORD APIENTRY VerLanguageNameA_bridge(DWORD wLang, LPSTR szLang, DWORD nSize)
{
	using Func = decltype(&VerLanguageNameA);
	static Func VerLanguageNameA = (Func)GetProcAddress
		(hDll, "VerLanguageNameA");
	return VerLanguageNameA(wLang, szLang, nSize);
}

// VerLanguageNameW
DWORD APIENTRY VerLanguageNameW_bridge(DWORD wLang, LPWSTR szLang, DWORD nSize)
{
	using Func = decltype(&VerLanguageNameW);
	static Func VerLanguageNameW = (Func)GetProcAddress
		(hDll, "VerLanguageNameW");
	return VerLanguageNameW(wLang, szLang, nSize);
}

// VerQueryValueA
BOOL APIENTRY VerQueryValueA_bridge(const LPVOID pBlock, LPCSTR lpSubBlock,
	LPVOID* lplpBuffer, PUINT puLen)
{
	using Func = decltype(&VerQueryValueA);
	static Func VerQueryValueA = (Func)GetProcAddress
		(hDll, "VerQueryValueA");
	return VerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
}

// VerQueryValueW
BOOL APIENTRY VerQueryValueW_bridge(const LPVOID pBlock, LPCWSTR lpSubBlock,
	LPVOID* lplpBuffer, PUINT puLen)
{
	using Func = decltype(&VerQueryValueW);
	static Func VerQueryValueW = (Func)GetProcAddress
		(hDll, "VerQueryValueW");
	return VerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen);
}

_int64 GetFileVersionInfoByHandle_bridge()
{
	using Func = decltype(&GetFileVersionInfoByHandle_bridge);
	static Func GetFileVersionInfoByHandle = (Func)GetProcAddress
		(hDll, "GetFileVersionInfoByHandle");
	return GetFileVersionInfoByHandle();
}

