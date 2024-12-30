module;
#include <ntdll.h>

export module reg_common;
import std;
//import func2hook.kernel.raw;
export {
  std::wstring GetKeyPath(HKEY hKey) {
    // 初始缓冲区大小
    ULONG bufferSize = 0;
    NtQueryKey(hKey, KeyNameInformation, nullptr, 0, &bufferSize);
    if (bufferSize == 0) {
      throw std::runtime_error("NtQueryKey 失败。\n");
    }

    // 使用std::unique_ptr管理缓冲区
    std::unique_ptr<UINT8[]> buffer(new UINT8[bufferSize]);
    if (!buffer) {
      throw std::runtime_error("内存分配失败。\n");
    }

    NTSTATUS status = NtQueryKey(hKey, KeyNameInformation, buffer.get(),
                                 bufferSize, &bufferSize);
    if (status == 0) {  // STATUS_SUCCESS
      PKEY_NAME_INFORMATION keyNameInfo = (PKEY_NAME_INFORMATION)(buffer.get());
      return std::wstring(keyNameInfo->Name, keyNameInfo->NameLength / 2);
    } else {
      throw std::runtime_error(
          std::format("NtQueryKey 失败，状态码: 0x%X\n", status));
    }
  }
}