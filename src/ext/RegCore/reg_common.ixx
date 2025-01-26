module;
#include <ntdll.h>

export module reg_common;
import std;
import my_converter.str;

// import func2hook.kernel.raw;
export {
  constexpr wchar_t REG_PREFIX[] = LR"(\REGISTRY\)";

  constexpr int REG_PREFIX_LEN = std::size(REG_PREFIX) - 1;

  std::wstring GetKeyPath(HKEY hKey) {
    std::unordered_map<HKEY, std::wstring> excluded(
        {{HKEY_LOCAL_MACHINE, L"HKEY_LOCAL_MACHINE"},
         {HKEY_CURRENT_USER, L"HKEY_CURRENT_USER"},
         {HKEY_USERS, L"HKEY_USERS"},
         {HKEY_CLASSES_ROOT, L"HKEY_CLASSES_ROOT"},
         {HKEY_CURRENT_CONFIG, L"HKEY_CURRENT_CONFIG"}});
    if (excluded.contains(hKey)) {
      return excluded.at(hKey);
    };
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
  std::unique_ptr<wchar_t[]> strConvert(const char* str, size_t bufSize = -1) {
    if (str == nullptr)
      return nullptr;
    std::wstring wstr = brv::strConvert(std::string_view(str));
    size_t bufSizeFinal = bufSize == -1 ? wstr.size() + 1 : bufSize;
    std::unique_ptr<wchar_t[]> ret(new wchar_t[bufSizeFinal]);
    std::wmemcpy(ret.get(), wstr.c_str(), bufSizeFinal);
    return ret;
  }
  std::wstring AnsiToWide(const std::string& str) {
    if (str.empty())
      return std::wstring();
    int needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    if (needed <= 0)
      return std::wstring();
    std::wstring wstr(needed - 1, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], needed);
    return wstr;
  }
  std::string WideToAnsi(const std::wstring& wstr) {
    if (wstr.empty())
      return std::string();
    int needed =
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (needed <= 0)
      return std::string();
    std::string str(needed - 1, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], needed, NULL,
                        NULL);
    return str;
  }
}