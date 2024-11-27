module;
#include <Windows.h>

export module selfDir;
import std;
using namespace std;

namespace fs = filesystem;
fs::path selfDirInternal;
export const fs::path& selfDir=selfDirInternal;

extern "C" BOOL DllMain(HMODULE hMoudle, DWORD dwReason, LPVOID lpReserved);
static struct Init {
  Init() {
    HMODULE hModule = nullptr;

    // 使用 GetModuleHandleExW 获取当前模块句柄
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&DllMain), &hModule)) {
      wchar_t buf[MAX_PATH];
      GetModuleFileNameW(hModule, buf, size(buf));

      selfDirInternal = fs::path(buf).parent_path();
    }
  }
} init;