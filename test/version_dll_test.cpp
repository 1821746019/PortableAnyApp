#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;
int main()
{
	vector<int> tmp;

	DWORD dwSize = 0;
	DWORD dwHandle = 0;
	LPVOID lpData;
	VS_FIXEDFILEINFO* pFileInfo;
	cout << &GetFileVersionInfoSizeA << endl;
	cout << &GetFileVersionInfoA << endl;
//	getchar();
	// 获取版本信息大小
	dwSize = GetFileVersionInfoSizeA("example.dll", &dwHandle);
	if (dwSize == 0)
	{
		printf("Error %d\n", GetLastError());
		return 1;
	}

	// 分配内存
	lpData = malloc(dwSize);

	// 获取版本信息
	if (!GetFileVersionInfoA("example.dll", dwHandle, dwSize, lpData))
	{
		printf("Error %d\n", GetLastError());
		return 1;
	}

	UINT uLen;
	// 查询特定的版本信息
	if (VerQueryValueA(lpData, "\\", (LPVOID*)&pFileInfo, &uLen))
	{
		printf("File Version: %d.%d.%d.%d\n",
			(pFileInfo->dwFileVersionMS >> 16) & 0xffff,
			(pFileInfo->dwFileVersionMS >> 0) & 0xffff,
			(pFileInfo->dwFileVersionLS >> 16) & 0xffff,
			(pFileInfo->dwFileVersionLS >> 0) & 0xffff);
	}

	// 释放内存
	free(lpData);

	return 0;
}