module;
#include <ntdll.h>
export module _;
import std;
using namespace std;
extern "C" int main() {
  // 打开一个注册表键
  HKEY hKey;
  LONG lResult = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                              L"Software\\Microsoft\\Windows\\CurrentVersion",
                              0, KEY_READ, &hKey);


  // 准备查询的参数
  UNICODE_STRING valueName;
  RtlInitUnicodeString(&valueName, ( wchar_t*)L"ProgramFilesDir");

  ULONG bufferLength = 1024;
  BYTE buffer[1024];  // 缓冲区用于接收返回的信息
  ULONG resultLength = 0;

  // 调用 NtQueryValueKey
  NTSTATUS status = NtQueryValueKey((HANDLE)hKey,             // 注册表项句柄
                                    &valueName,               // 值名
                                    KeyValueFullInformation,  // 信息类型
                                    buffer,        // 存储结果的缓冲区
                                    bufferLength,  // 缓冲区大小
                                    &resultLength  // 返回实际数据长度
  );

  if (status == 0) {  // 0 表示 STATUS_SUCCESS
    std::wcout << L"Successfully queried the registry value." << std::endl;
    // 在这里你可以解析 buffer 来获取注册表值的内容
    // 需要注意的是 buffer 的内容需要根据 KeyValueFullInformation 结构来解析
  } else {
    std::cerr << "NtQueryValueKey failed!" << std::endl;
  }

  // 关闭注册表项句柄
  RegCloseKey(hKey);
}