#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;

size_t __cdecl bs_strlen(const char* Str) {
  const char* v1;       // rax
  __int64 v2;           // rcx
  __int64 v4;           // rdx
  unsigned __int64 v5;  // rdx
  unsigned __int64 v6;  // rdx
  unsigned __int64 v7;  // rdx
  unsigned int v8;      // edx

  v1 = Str;
  v2 = -(__int64)Str;
  if (((unsigned __int8)v1 & 7) == 0)
    goto main_loop_begin;
  do {
    if (!*v1++)
      return (size_t)&v1[v2 - 1];
  } while (((unsigned __int8)v1 & 7) != 0);
  while (1) {
    do {
    main_loop_begin:
      v4 = *(__int64*)v1;
      v1 += 8;
    } while ((((v4 + 0x7EFEFEFEFEFEFEFFLL) ^ ~v4) & 0x8101010101010100uLL) == 0);
    v5 = *((__int64*)v1 - 1);
    if (!v5)
      break;
    if (!(v5))
      return (size_t)&v1[v2 - 7];
    v6 = v5 >> 16;
    if (!v6)
      return (size_t)&v1[v2 - 6];
    if (!(v6))
      return (size_t)&v1[v2 - 5];
    v7 = v6 >> 16;
    if (!v7)
      return (size_t)&v1[v2 - 4];
    if (!(v7))
      return (size_t)&v1[v2 - 3];
    v8 = (v7);
    if (!v8)
      return (size_t)&v1[v2 - 2];
    if (!(v8))
      return (size_t)&v1[v2 - 1];
  }
  return (size_t)&v1[v2 - 8];
}
int main() {
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
  if (dwSize == 0) {
    printf("Error %d\n", GetLastError());
    return 1;
  }

  // 分配内存
  lpData = malloc(dwSize);

  // 获取版本信息
  if (!GetFileVersionInfoA("example.dll", dwHandle, dwSize, lpData)) {
    printf("Error %d\n", GetLastError());
    return 1;
  }

  UINT uLen;
  // 查询特定的版本信息
  if (VerQueryValueA(lpData, "\\", (LPVOID*)&pFileInfo, &uLen)) {
    printf(
        "File Version: %d.%d.%d.%d\n", (pFileInfo->dwFileVersionMS >> 16) & 0xffff,
        (pFileInfo->dwFileVersionMS >> 0) & 0xffff, (pFileInfo->dwFileVersionLS >> 16) & 0xffff,
        (pFileInfo->dwFileVersionLS >> 0) & 0xffff
    );
  }

  // 释放内存
  free(lpData);

  return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;
  cout<<bs_strlen("666");
  return TRUE;
}