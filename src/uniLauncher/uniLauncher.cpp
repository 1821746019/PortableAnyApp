#include <toml++/toml.hpp>
import std;
#include <Windows.h>
import helper;
using namespace std;

bool InjectDLL(HANDLE hProcess, const char* dllPath) {
  // 获取LoadLibraryA的地址
  LPVOID pLoadLibrary =
      (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryA");
  if (!pLoadLibrary) {
    std::cerr << "无法获取LoadLibraryA地址" << std::endl;
    return false;
  }

  // 分配内存给目标进程以存储DLL路径
  LPVOID pRemoteString =
      VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1,
                     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if (!pRemoteString) {
    std::cerr << "无法分配内存" << std::endl;
    return false;
  }

  // 写入DLL路径到目标进程
  if (!WriteProcessMemory(hProcess, pRemoteString, dllPath, strlen(dllPath) + 1,
                          nullptr)) {
    std::cerr << "无法写入目标进程内存" << std::endl;
    VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
    return false;
  }

  // 在目标进程中创建远程线程，调用LoadLibraryA加载DLL
  HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                      (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                      pRemoteString, 0, nullptr);
  if (!hThread) {
    std::cerr << "无法创建远程线程" << std::endl;
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
void launchExeAndInjectDLl(const filesystem::path& exePath,const filesystem::path& dllPath)
{


  // 创建目标进程test.exe
  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi;

  if (!CreateProcessW(exePath.wstring().data(), nullptr, nullptr, nullptr, FALSE,
                     CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
    throw runtime_error("无法创建test.exe进程");
    
  }

  // 注入DLL
  if (!InjectDLL(pi.hProcess, dllPath.string().data())) {
    TerminateProcess(pi.hProcess, 0);
    throw runtime_error( "DLL注入失败" );
    
  }

  // 恢复目标进程执行
  ResumeThread(pi.hThread);

  // 等待目标进程结束
  WaitForSingleObject(pi.hProcess, INFINITE);

  // 清理资源
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
}

int main() {
  toml::parse_result config = toml::parse_file("uniLaucher.toml");
  wstring AppPath = config["AppPath"].value_or(L"");

  PECheckStatus status = CheckPE(AppPath);
  if (status == PECheckStatus::PE64) {
    launchExeAndInjectDLl(AppPath, "extBoot.x64.dll");
  } else if (status == PECheckStatus::PE32) {

    launchExeAndInjectDLl(AppPath, "extBoot.x86.dll");
  } else {
    MessageBoxW(nullptr, L"Please check the AppPath", L"Error", MB_ICONERROR);
  }
}
