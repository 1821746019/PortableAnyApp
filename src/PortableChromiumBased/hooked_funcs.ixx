module;
#include <Windows.h>
#include <string>
export module hooked_funcs;
import CmdlineMgr;
import my_converter.str;
import shared;
//import std;
using namespace std;
// using std::string;
__int64 __fastcall CreateProcessInternalW(
    void* hToken,  // a1 - 表示调用者的令牌或访问标志
    const WCHAR* lpApplicationName,  // lpApplicationName - 可执行文件名称
    const WCHAR* lpCommandLine,      // lpCommandLine - 命令行参数
    WCHAR* lpProcessAttributes,      // - 新进程的安全属性
    WCHAR* lpThreadAttributes,       // - 新线程的安全属性
    int bInheritHandles,             // - 是否继承句柄
    int dwCreationFlags,             // - 进程创建标志
    __int64 lpEnvironment,           // - 环境变量
    const WCHAR* lpCurrentDirectory,           //  - 当前目录
    STARTUPINFOW* lpStartupInfo,               //  - 启动信息
    PROCESS_INFORMATION* lpProcessInformation  //  - 返回进程信息
);

bool isNeedLoading(auto arg) {
  return false;
}
export {
  string exe_path_quoted_if_need = quotePathIfNeed(getNowExePath());
  using CreateProcessA_t = decltype(&CreateProcessA);
  CreateProcessA_t CreateProcessA_raw = &CreateProcessA;

  BOOL WINAPI CreateProcessA_mod(
      LPCSTR lpApplicationName, LPSTR lpCommandLine,
      LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
      DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
      LPSTARTUPINFOA lpStartupInfo,
      LPPROCESS_INFORMATION lpProcessInformation) {
    if (!string_view(lpCommandLine).starts_with(exe_path_quoted_if_need)) {
      return CreateProcessA_raw(
          lpApplicationName, lpCommandLine, lpProcessAttributes,
          lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
          lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }
    string cmdline_new;
    procCmdline(lpCommandLine, &cmdline_new);
    return CreateProcessA_raw(
        lpApplicationName, cmdline_new.data(), lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
  }
  decltype(&CreateProcessInternalW) CreateProcessInternalW_raw =
      (decltype(&CreateProcessInternalW))GetProcAddress(
          LoadLibraryA("kernelbase.dll"), "CreateProcessInternalW");

  __int64 __fastcall CreateProcessInternalW_mod(
      void* hToken,  // a1 - 表示调用者的令牌或访问标志
      const WCHAR* lpApplicationName,  // lpApplicationName - 可执行文件名称
      const WCHAR* lpCommandLine,      // lpCommandLine - 命令行参数
      WCHAR* lpProcessAttributes,      // - 新进程的安全属性
      WCHAR* lpThreadAttributes,       // - 新线程的安全属性
      int bInheritHandles,             // - 是否继承句柄
      int dwCreationFlags,             // - 进程创建标志
      __int64 lpEnvironment,           // - 环境变量
      const WCHAR* lpCurrentDirectory,           // - 当前目录
      STARTUPINFOW* lpStartupInfo,               //  - 启动信息
      PROCESS_INFORMATION* lpProcessInformation  // - 返回进程信息
  ) {
    if (!isNeedLoading(lpCommandLine)) {
      return CreateProcessInternalW_raw(
          hToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
          lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
          lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    }
    // dwCreationFlags |= CREATE_SUSPENDED;
    auto ret = CreateProcessInternalW_raw(
        hToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    PROCESS_INFORMATION& pi = *lpProcessInformation;

    return ret;
  }
}