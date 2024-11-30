module;
#include <Windows.h>

export module selfDir;
import std;
using namespace std;

namespace fs = filesystem;

extern "C" BOOL DllMain(HMODULE hMoudle, DWORD dwReason, LPVOID lpReserved);
export const fs::path selfDir = [] {
  HMODULE hModule = nullptr;

  // 使用 GetModuleHandleExW 获取当前模块句柄
  if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                             GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                         reinterpret_cast<LPCWSTR>(&DllMain), &hModule)) {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(hModule, buf, size(buf));

    return fs::path(buf).parent_path();
  } else {
    throw runtime_error("Failed to get the module handle");
  }
}();

;