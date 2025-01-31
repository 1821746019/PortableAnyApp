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
    static std::unordered_map<HKEY, std::wstring> excluded({
        {HKEY_LOCAL_MACHINE, L"HKEY_LOCAL_MACHINE"},
        {HKEY_CURRENT_USER, L"HKEY_CURRENT_USER"},
        {HKEY_USERS, L"HKEY_USERS"},
        {HKEY_CLASSES_ROOT, L"HKEY_CLASSES_ROOT"},
        {HKEY_CURRENT_CONFIG, L"HKEY_CURRENT_CONFIG"},
        {HKEY_DYN_DATA, L"HKEY_DYN_DATA"},
        {HKEY_CURRENT_USER_LOCAL_SETTINGS, L"HKEY_CURRENT_USER_LOCAL_SETTINGS"},
        ////最后这两个是必须的，防止调用方传入该hkey, NtQueryKey失败导致异常抛出(他妈VS定位throw
        // runtime_error也太不准了，debug几小时最后无意step-in了几步才发现是GeyKeyPath抛出的
        //  坑啊 Frames below may be incorrect and/or missing, no symbols loaded for vcruntime140d.dll
        //  加载vcruntime140d.dll.pdb解决runtime_error throw位置定位不准的问题
    });

    if (auto it = excluded.find(hKey); it != excluded.end()) {
      return it->second;
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

    NTSTATUS status = NtQueryKey(hKey, KeyNameInformation, buffer.get(), bufferSize, &bufferSize);
    if (status == 0) {  // STATUS_SUCCESS
      PKEY_NAME_INFORMATION keyNameInfo = (PKEY_NAME_INFORMATION)(buffer.get());
      return std::wstring(keyNameInfo->Name, keyNameInfo->NameLength / 2);
    } else {
      throw std::runtime_error(std::format("NtQueryKey 失败，状态码: 0x%X\n", status));
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
    int needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (needed <= 0)
      return std::string();
    std::string str(needed - 1, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], needed, NULL, NULL);
    return str;
  }
  // e.用于把形如 "MACHINE\\..." 替换为 "HKEY_LOCAL_MACHINE\\..."
  std::wstring abstractNonRootFromAbsRegPath(const std::wstring_view& src) {
    if (!src.starts_with(REG_PREFIX))
      return src.data();
    std::wstring NoAbsPrefix(src.data() + REG_PREFIX_LEN);
    // 判断并替换前缀
    if (NoAbsPrefix.starts_with(L"MACHINE\\")) {
      return L"HKEY_LOCAL_MACHINE\\" + NoAbsPrefix.substr(8);  // 去掉 "MACHINE\" (长度8)
    } else if (NoAbsPrefix.starts_with(L"USER\\")) {
      return L"HKEY_CURRENT_USER\\" + NoAbsPrefix.substr(5);  // 去掉 "USER\" (长度5)
    }

    // 如果既不是 MACHINE\ 也不是 USER\，直接返回
    return NoAbsPrefix;
  }
}