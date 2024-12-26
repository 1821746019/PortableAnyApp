module;
#include <Windows.h>
export module log;

import std;

export {
  void VSDebugOutput(const std::string_view& msg) {
#ifdef BS_DBG
    OutputDebugStringA(msg.data());
#endif

  }
  void VSDebugOutput(const std::wstring_view& msg) {
#ifdef BS_DBG
    OutputDebugStringW(msg.data());
#endif
  }
}