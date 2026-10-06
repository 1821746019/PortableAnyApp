module;

#include <Windows.h>
#include <wchar.h>

export module _;

import std;
import Hooker;
using namespace std;

// 可在二进制中以 UTF-8 直接搜到并编辑：8 行 × 128 字节，行间 stride = 128 字节。
// 仅第 0 行有值；空行（首字节为 '\0'）不参与匹配。
// 必须是非 const 的文件作用域数组：const/字面量会让编译器把 isBlockedTitle 的比较整个折叠掉，补丁失效。
static char g_blockedTitlePrefixesUtf8[8][128] = {"Demo version"};

bool isBlockedTitle(const wchar_t* title) {
  if (title == nullptr)
    return false;

  for (auto& utf8 : g_blockedTitlePrefixesUtf8) {
    if (utf8[0] == '\0')  // 空行 = 未配置；若不跳过，空前缀会匹配一切
      continue;

    wchar_t prefix[128];
    // -1 表示以 '\0' 结束，返回值包含结尾的 '\0'
    int prefixLen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, prefix, size(prefix));
    if (prefixLen <= 1)
      continue;

    if (_wcsnicmp(title, prefix, prefixLen - 1) == 0)  // 不区分大小写的 starts with
      return true;
  }
  return false;
}

decltype(&MessageBoxW) MessageBoxW_raw = &MessageBoxW;

int WINAPI MessageBoxW_mod(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
  if (isBlockedTitle(lpCaption))
    return IDOK;  // 伪造“用户点击了确定”，弹窗不出现
  return MessageBoxW_raw(hWnd, lpText, lpCaption, uType);
}

void setHook() {
  DetoursHooker hooker;
  hooker.endeque({{&MessageBoxW_raw, &MessageBoxW_mod}});
  hooker.setHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReson, LPVOID lpReserved) {
  if (dwReson != DLL_PROCESS_ATTACH)
    return TRUE;

  DisableThreadLibraryCalls(hModule);

  try {
    setHook();
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception occured", MB_ICONERROR);
    exit(-1);
  }
  return TRUE;
}
