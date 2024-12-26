#include <Windows.h>
#include <toml++/toml.h>

import std;
import Hooker;
using namespace std;
namespace fs = filesystem;

import selfDir;
bool InjectDLL(HANDLE hProcess, const char* dllPath) {
  // 获取LoadLibraryA的地址
  LPVOID pLoadLibrary =
      (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryA");
  if (!pLoadLibrary) {
    cerr << "无法获取LoadLibraryA地址" << endl;
    return false;
  }

  // 分配内存给目标进程以存储DLL路径
  LPVOID pRemoteString =
      VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1,
                     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (!pRemoteString) {
    cerr << "无法分配内存" << endl;
    return false;
  }

  // 写入DLL路径到目标进程
  if (!WriteProcessMemory(hProcess, pRemoteString, dllPath, strlen(dllPath) + 1,
                          nullptr)) {
    cerr << "无法写入目标进程内存" << endl;
    VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
    return false;
  }

  // 在目标进程中创建远程线程，调用LoadLibraryA加载DLL
  HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                      (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                      pRemoteString, 0, nullptr);
  if (!hThread) {
    cerr << "无法创建远程线程" << endl;
    VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
    return false;
  }

  // 等待线程完成
  WaitForSingleObject(hThread, INFINITE);

  // 释放内存
  VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
  CloseHandle(hThread);

  return true;
}
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

bool isNeedLoading(wstring_view cmdline) {
  // reduce the usage of global var as much as possible, make it static so it
  // just be inited once
  static toml::parse_result config =
      toml::parse_file((selfDir() / "loadDllInChildren.toml").wstring());
  // 获取配置
  bool isBlackListMode = config["isBlacklistMode"].as_boolean();
  auto str2wstr = [](const string& e) { return fs::path(e).wstring(); };
  vector<wstring> excluded_list;
  for (auto& e : *(config["excluded_list"].as_array())) {
    excluded_list.emplace_back(e.value<wstring>().value());
  }
  vector<wstring> included_list;
  for (auto& e : *config["included_list"].as_array()) {
    included_list.emplace_back(e.value<wstring>().value());
  }

  // 定义正则表达式向量
  vector<wregex> excluded_re, included_re;

  // 转换函数，将字符串转换为 wregex
  auto transFunc = [&](wstring str) -> wregex {
    // 检查字符串是否以 '/' 开头和结尾
    if (str.starts_with(L"/") && str.ends_with(L"/")) {
      // 提取中间的内容
      str = str.substr(1, str.length() - 2);
    } else {
      // indicate that it is not a regex pattern
      //  替换所有的 '.' 为 '\.'
      size_t pos;
      // 替换所有/
      pos = 0;
      while ((pos = str.find(LR"(\)", pos)) != wstring::npos) {
        str.replace(pos, 1, LR"(\\)");
        pos += 2;  // 跳过新替换的字符
      }
      pos = 0;
      while ((pos = str.find(L".", pos)) != wstring::npos) {
        str.replace(pos, 1, LR"(\.)");
        pos += 2;  // 跳过新替换的字符
      }
    }

    // 创建并返回 wregex 对象
    return wregex(str);
  };
  // 转换 excluded_list 和 included_list 为正则表达式
  ranges::transform(excluded_list, back_inserter(excluded_re), transFunc);
  ranges::transform(included_list, back_inserter(included_re),
                    transFunc);  // 修正这里，插入到 in_re

  // 匹配过程名是否在黑名单或白名单中
  bool match_excluded = ranges::any_of(excluded_re, [&](const wregex& re) {
    return regex_search(cmdline.begin(), cmdline.end(), re);
  });

  bool match_included = ranges::any_of(included_re, [&](const wregex& re) {
    return regex_search(cmdline.begin(), cmdline.end(), re);
  });

  // 合并黑名单和白名单的逻辑
  if (isBlackListMode) {
    // 黑名单模式:
    // 如果匹配到黑名单，或者不匹配任何白名单，则不需要加载 括号内为true则匹配黑名单
    return !(!match_excluded && match_included);
  } else {
    // 白名单模式:
    // 如果匹配到白名单，或者不匹配任何黑名单，则需要加载
    return !(!match_excluded || match_included);
  }
}
decltype(&CreateProcessInternalW) CreateProcessInternalW_raw =
    (decltype(&CreateProcessInternalW))GetProcAddress(
        LoadLibraryA("kernelbase.dll"),
        "CreateProcessInternalW");

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
  dwCreationFlags |= CREATE_SUSPENDED;
  auto ret = CreateProcessInternalW_raw(
      hToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
      lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
      lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
  PROCESS_INFORMATION& pi = *lpProcessInformation;

  if (!ret) {
    throw runtime_error("无法创建test.exe进程");
  }
  fs::path dllPath = [] {
    const char* ret = getenv("EXT_BOOT_PATH");
    return !ret ? string() : string(ret);
  }();
  // 注入DLL
  if (!InjectDLL(pi.hProcess, dllPath.string().data())) {
    TerminateProcess(pi.hProcess, 0);
    throw runtime_error("DLL注入失败");
  }

  // 恢复目标进程执行
  ResumeThread(pi.hThread);

  return ret;
}
void enableLoadDllInChildren() {
  DetoursHooker hooker;
  hooker.endeque({&CreateProcessInternalW_raw, CreateProcessInternalW_mod});
  hooker.setHook();
}

BOOL DllMain(HMODULE hMoudle, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    enableLoadDllInChildren();
  } else if (dwReason == DLL_PROCESS_DETACH) {
  }

  return TRUE;
}
