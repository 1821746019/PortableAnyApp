module;
// #include <Windows.h>
export module RegGuardConfig;
import std;
import GuardConfig;
// import ConfigMgr;
export class RegGuardConfig {
  RegGuardConfig() {}
  inline static std::unique_ptr<GuardConfig> ins_;

 public:
  // 单例模式
  RegGuardConfig(const RegGuardConfig&) = delete;
  RegGuardConfig& operator=(const RegGuardConfig&) = delete;
  RegGuardConfig(RegGuardConfig&&) = delete;
  RegGuardConfig& operator=(RegGuardConfig&&) = delete;
  static  GuardConfig& _ins_() {
    if (ins_)
      return *ins_;

    ins_ = GuardConfig::_createIns_();

    return *ins_;
  }
};