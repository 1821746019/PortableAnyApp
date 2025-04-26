module;
#include <Windows.h>
export module RegHandler_Redirect;

import std;
import RegHandler_B;
import reg_common;
import RegGuardConfig;
import func2hook.kernel.raw;
import AppRegHive;
import strUtils;

auto& regGuardConfig = RegGuardConfig::_ins_();
bool isNeedRedirection(const std::wstring& path) {
  bool ret = false;
  decltype(regGuardConfig.open(path, nullptr)) context = nullptr;
  auto key = regGuardConfig.open(path, &context);
  if ((key && key->keyInfo->redirect) || context && context->keyInfo->redirect) {
    ret = true;
  }
  return ret;
}
std::vector<std::wstring> excluded{
    L"sogou",
},
    included{
        L"Manager",
    };
#ifdef BS_DBG
// 定义宏 retrunIfExcluded
#define returnIfExcluded(call)                     \
  for (const auto& e : excluded) {                 \
    if (wcsistr(path_old_full.data(), e.data())) { \
      return call;                                 \
    }                                              \
  }
#define returnIfNotIncluded(call)                   \
  for (const auto& e : included) {                  \
    if (!wcsistr(path_old_full.data(), e.data())) { \
      return call;                                  \
    }                                               \
  }
#else
#define returnIfExcluded(call)
#endif

bool isInIncluded(const std::wstring& path) {
  for (const auto& e : included) {
    if (wcsistr(path.data(), e.data())) {
      return true;
    }
  }
  return false;
}

export class RegHandler_Redirect : public RegHandler_B {
 public:
  LSTATUS RegCreateKeyExInternalW(
      HKEY hKey,
      LPCWSTR lpSubKey,
      DWORD Reserved,
      LPWSTR lpClass,
      DWORD dwOptions,
      REGSAM samDesired,
      LPSECURITY_ATTRIBUTES lpSecurityAttributes,
      PHKEY phkResult,
      LPDWORD lpdwDisposition,
      LPVOID lpReserved
  ) override {
    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));
    std::wstring path_old_full = path_old + L"\\" + (lpSubKey ? lpSubKey : L"");
    // 检查是否在排除列表中
    bool isIncluded = isInIncluded(path_old_full);

    if (isNeedRedirection(path_old_full)) {
      return RegCreateKeyExInternalW_raw(
          getCachedAppHiveRootKey(), path_old_full.data(), Reserved, lpClass, dwOptions, samDesired,
          lpSecurityAttributes, phkResult, lpdwDisposition, lpReserved
      );
    }
    return RegCreateKeyExInternalW_raw(
        hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult,
        lpdwDisposition, lpReserved
    );
  };
  LSTATUS
  RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) override {
    std::wstring path_old = abstractNonRootFromAbsRegPath(GetKeyPath(hKey));
    std::wstring path_old_full = path_old + L"\\" + (lpSubKey ? lpSubKey : L"");
    bool isIncluded = isInIncluded(path_old_full);

    if (isNeedRedirection(path_old_full)) {
      return RegOpenKeyExW_raw(getCachedAppHiveRootKey(), path_old_full.data(), ulOptions, samDesired, phkResult);
    }
    return RegOpenKeyExW_raw(hKey, lpSubKey, ulOptions, samDesired, phkResult);
  };
};