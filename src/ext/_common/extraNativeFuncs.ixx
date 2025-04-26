module;
#include <Windows.h>

export module extraNativeFuncs;

import std;

export {
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
  decltype(&CreateProcessInternalW) CreateProcessInternalW_raw = (decltype(&CreateProcessInternalW)
  )GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "CreateProcessInternalW");
}