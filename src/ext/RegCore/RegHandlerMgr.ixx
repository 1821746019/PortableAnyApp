module;
#include <Windows.h>
export module RegHandlerMgr;

import std;
import ConfigMgr;
import RegHandler_B;
import RegHandler_Virtual;
import RegHandler_Patch;
export class RegHandlerMgr {
 public:
 private:
  RegHandlerMgr() {
    std::unordered_map<ConfigMgr::RegModeEnum,
                       std::function<std::unique_ptr<RegHandler_B>()>>
        map{{ConfigMgr::Patch, std::make_unique<RegHandler_Patch>},
            {ConfigMgr::Virtual, std::make_unique<RegHandler_Virtual>}};
    RegHandler_ = map[ConfigMgr::_ins_().RegMode()]();
  };  // 私有构造函数，禁止外部直接创建实例
  RegHandlerMgr(const RegHandlerMgr&) = delete;
  RegHandlerMgr& operator=(const RegHandlerMgr&) = delete;

  inline static std::unique_ptr<RegHandlerMgr> ins_{nullptr};
  // 工厂模式
  static auto createIns() {
    auto regHandlerMgr = std::unique_ptr<RegHandlerMgr>(new RegHandlerMgr());

    return regHandlerMgr;
  }
  std::unique_ptr<RegHandler_B> RegHandler_{nullptr};

 public:
  RegHandler_B& RegHandler() const { return *RegHandler_; }
  static RegHandlerMgr& _reinit_() {
    ins_ = createIns();
    return *ins_.get();
  }
  static RegHandlerMgr& _ins_() {
    if (!ins_) {
      ins_ = createIns();
    }
    return *ins_;
  }
};