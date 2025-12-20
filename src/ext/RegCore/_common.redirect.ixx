module;
#include <Windows.h>
export module _common.redirect;
import std;
import RegGuardConfig;
import strUtils;
#define regGuardConfig RegGuardConfig::_ins_()
std::vector<std::wstring> excluded{
    L"sogou",
},
    included{
        L"Cryptography",
    };
export {
  bool isNeedRedirection(const std::wstring& path) {
    bool tmp = path.contains(LR"(HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\GRE_Initialize)");
    if (tmp) {
      std::println("{}", tmp);
    }
    bool ret = false;
    decltype(regGuardConfig.open(path, nullptr)) context = nullptr;
    auto key = regGuardConfig.open(path, &context);
    
    if ((key && key->keyInfo->redirect) || context && context->keyInfo->redirect) {
      ret = true;
    }
    return ret;
  }

  bool isInIncluded(const std::wstring& path) {
    for (const auto& e : included) {
      if (wcsistr(path.data(), e.data())) {
        return true;
      }
    }
    return false;
  }
}