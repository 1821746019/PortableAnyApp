module;
#include <Windows.h>
export module selfInfo;

import std;
// using namespace std;

namespace fs = std::filesystem;

// extern "C" BOOL DllMain(HMODULE hMoudle, DWORD dwReason, LPVOID lpReserved);
export fs::path selfPath() {
  static const fs::path ret = [] {
    HMODULE hModule = nullptr;
    // 使用 GetModuleHandleExW
    // 获取当前模块句柄，传入本函数的地址，本ixx若被dll链接则会获得dll的句柄,
    // 若被exe链接则会获得exe的句柄
    if (GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&selfPath), &hModule
        )) {
      wchar_t buf[MAX_PATH];
      GetModuleFileNameW(hModule, buf, std::size(buf));

      fs::path ret = fs::path(buf);
      return ret;
    } else {
      throw std::runtime_error("Failed to get the module handle");
    }
  }();
  return ret;
};
export fs::path selfDir() {
  return selfPath().parent_path();
};
export fs::path selfExePath() {
  wchar_t exe_path2dir[MAX_PATH];
  GetModuleFileNameW(nullptr, exe_path2dir, std::size(exe_path2dir));
  return exe_path2dir;
}
export fs::path selfExeDir() {
  return selfExePath().parent_path();
}