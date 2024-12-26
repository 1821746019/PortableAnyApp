module;
#include <shobjidl_core.h>
#include <Windows.h>
export module func2hook;

import std;

export {
  decltype(&SetCurrentProcessExplicitAppUserModelID)
      SetCurrentProcessExplicitAppUserModelID_raw =
          &SetCurrentProcessExplicitAppUserModelID;
  auto WINAPI SetCurrentProcessExplicitAppUserModelID_mod(PWSTR AppID) {
    std::wstring exePath =
        [] {
          wchar_t exePath[MAX_PATH];
          GetModuleFileNameW(nullptr, exePath, std::size(exePath));
          return std::wstring(exePath);
        }()
            .data();

    return SetCurrentProcessExplicitAppUserModelID_raw(
        exePath.data());
  }
}