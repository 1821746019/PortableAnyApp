module;
#include <cwchar>
#include <windows.h>
export module strUtils;
import std;
export {
  const wchar_t* wcsistr(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle)
      return nullptr;

    size_t needleLen = wcslen(needle);
    while (*haystack) {
      // 对比当前窗口的子字符串是否相等（忽略大小写）
      if (_wcsnicmp(haystack, needle, needleLen) == 0) {
        return haystack;
      }
      haystack++;
    }
    return nullptr;
  }
  void to_lowercase(wchar_t * in) {
    for (int i = 0; in[i]; i++) {
      in[i] = std::towlower(in[i]);
    }
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
}