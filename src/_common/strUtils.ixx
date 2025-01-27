module;
#include <cwchar>
export module strUtils;
//import std.compat;
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
}