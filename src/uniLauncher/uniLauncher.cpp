#include <Windows.h>

#include <toml++/toml.hpp>
import std;
import helper;
using namespace std;

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
void launchExeAndInjectDLl(const filesystem::path& exePath,
                           const filesystem::path& dllPath) {
  // 创建目标进程test.exe
  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi;

  if (!CreateProcessW(exePath.wstring().data(), nullptr, nullptr, nullptr,
                      FALSE, CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP,
                      nullptr, nullptr, &si, &pi)) {
    throw runtime_error("无法创建test.exe进程");
  }

  // 注入DLL
  if (!InjectDLL(pi.hProcess, dllPath.string().data())) {
    TerminateProcess(pi.hProcess, 0);
    throw runtime_error("DLL注入失败");
  }

  // 恢复目标进程执行
  ResumeThread(pi.hThread);

  // 等待目标进程结束
  // WaitForSingleObject(pi.hProcess, INFINITE);

  // 清理资源
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
}

int main(int argc, char* argv[]) {
  toml::parse_result config = toml::parse_file("uniLaucher.toml");
  wstring AppPath = config["AppPath"].value_or(L"");

  // initEnvVarForChildren(config);
  PECheckStatus status = CheckPE(AppPath);
  if (status == PECheckStatus::PE64) {
    launchExeAndInjectDLl(AppPath, "extBoot.dll");
  } else if (status == PECheckStatus::PE32) {
    launchExeAndInjectDLl(AppPath, "extBoot.x86.dll");
  } else {
    MessageBoxW(nullptr, L"Please check the AppPath", L"Error", MB_ICONERROR);
  }
}
BOOL WinMain(HMODULE hModule, HMODULE parent, LPSTR cmdline, int show_cmd) {
  // 获取完整的命令行
  int argc;
  LPWSTR* argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
  // 将宽字符参数转换为多字节（假设使用 UTF-8 编码）
  vector<char*> argv(argc);
  for (int i = 0; i < argc; ++i) {  
    string buf = filesystem::path(argvw[i]).string();
    // 分配缓冲区并进行转换
    argv[i] = new char[buf.length() + 1];
    strcpy_s(argv[i], buf.length() + 1, buf.data());
  }

  // 调用 main 函数

  int result;
  try {
    result = main(argc, argv.data());
  } catch (const exception& e) {
    MessageBoxA(nullptr, e.what(), "Exception Occurred!", MB_ICONERROR);
  }

  // 释放分配的内存
  for (int i = 0; i < argc; ++i) {
    delete[] argv[i];
  }
  LocalFree((void*)argvw);
  //CreateWindowExW();
  return result;
}
