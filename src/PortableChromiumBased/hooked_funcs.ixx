module;
#include <Windows.h>
#include <string>
export module hooked_funcs;
import CmdlineMgr;
import my_converter.str;
import shared;
import std;
using namespace std;
// using std::string;
__int64 __fastcall CreateProcessInternalW(
    void* hToken,                              // a1 - 表示调用者的令牌或访问标志
    const WCHAR* lpApplicationName,            // lpApplicationName - 可执行文件名称
    const WCHAR* lpCommandLine,                // lpCommandLine - 命令行参数
    WCHAR* lpProcessAttributes,                // - 新进程的安全属性
    WCHAR* lpThreadAttributes,                 // - 新线程的安全属性
    int bInheritHandles,                       // - 是否继承句柄
    int dwCreationFlags,                       // - 进程创建标志
    __int64 lpEnvironment,                     // - 环境变量
    const WCHAR* lpCurrentDirectory,           //  - 当前目录
    STARTUPINFOW* lpStartupInfo,               //  - 启动信息
    PROCESS_INFORMATION* lpProcessInformation  //  - 返回进程信息
);

bool isNeedLoading(auto arg) {
  return false;
}
export {
  string exe_path_quoted_if_need = quotePathIfNeed(getNowExePath());
  decltype(&CreateProcessInternalW) CreateProcessInternalW_raw = (decltype(&CreateProcessInternalW)
  )GetProcAddress(GetModuleHandleA("kernelbase.dll"), "CreateProcessInternalW");

  // 获取可执行文件所在目录的上级目录下的Data\user_data路径
  std::wstring GetCustomUserDataDir() {
    wchar_t exePath[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, exePath, MAX_PATH);

    // 获取exe所在目录
    std::wstring exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of(L"\\");
    if (lastSlash != std::wstring::npos) {
      exeDir = exeDir.substr(0, lastSlash);
    }

    // 获取上级目录
    lastSlash = exeDir.find_last_of(L"\\");
    if (lastSlash != std::wstring::npos) {
      exeDir = exeDir.substr(0, lastSlash);
    }

    // 返回Data\user_data路径
    return exeDir + L"\\Data\\user_data";
  }

  // 修改命令行，替换--user-data-dir参数
  std::wstring ModifyCommandLine(const wchar_t* originalCmd) {
    std::wstring cmdLine = originalCmd;
    std::wregex pattern(L"--user-data-dir=\"[^\"]+\"");
    std::wstring replacement = L"--user-data-dir=\"" + GetCustomUserDataDir() + L"\"";

    // 查找并替换所有匹配项
    std::wstring result = std::regex_replace(cmdLine, pattern, replacement);

    return result;
  }

  // 检查是否需要修改命令行
  bool IsNeedModifying(const wchar_t* cmdLine) {
    if (!cmdLine)
      return false;

    std::wstring cmd = cmdLine;
    std::wregex pattern(L"--user-data-dir=\"[^\"]+\"");

    return std::regex_search(cmd, pattern);
  }

  int64_t __fastcall CreateProcessInternalW_mod(
      void* hToken,                              // a1 - 表示调用者的令牌或访问标志
      const WCHAR* lpApplicationName,            // lpApplicationName - 可执行文件名称
      const WCHAR* lpCommandLine,                // lpCommandLine - 命令行参数
      WCHAR* lpProcessAttributes,                // - 新进程的安全属性
      WCHAR* lpThreadAttributes,                 // - 新线程的安全属性
      int bInheritHandles,                       // - 是否继承句柄
      int dwCreationFlags,                       // - 进程创建标志
      __int64 lpEnvironment,                     // - 环境变量
      const WCHAR* lpCurrentDirectory,           // - 当前目录
      STARTUPINFOW* lpStartupInfo,               // - 启动信息
      PROCESS_INFORMATION* lpProcessInformation  // - 返回进程信息
  ) {
    if (!IsNeedModifying(lpCommandLine)) {
      // 如果不需要修改，直接调用原始函数
      return CreateProcessInternalW_raw(
          hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
          dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
      );
    }

    // 修改命令行参数
    std::wstring modifiedCmd = ModifyCommandLine(lpCommandLine);

    // 使用修改后的命令行创建进程
    auto ret = CreateProcessInternalW_raw(
        hToken, lpApplicationName, modifiedCmd.c_str(), lpProcessAttributes, lpThreadAttributes,
        bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo,
        lpProcessInformation
    );

    return ret;
  }
}