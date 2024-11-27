module;

#include <string>
#include <unordered_map>
export module VarInterpolationMgr;
import fs_common;
export {
  class VarInterpolationMgr {
    std::unordered_map<std::wstring, std::wstring> macro_map_;

   public:
    VarInterpolationMgr() {}
    VarInterpolationMgr(
        const std::unordered_map<std::wstring, std::wstring>& macro_map)
        : macro_map_(macro_map) {}
    void add(const std::pair<std::wstring, std::wstring>& macro2value) {
      macro_map_.emplace(macro2value);
    }
    void add(const std::unordered_map<std::wstring, std::wstring>& macro_map) {
      for (auto& [k, v] : macro_map) {
        // do not allow to overwrite the builtin macro or assign more than one
        // time
        if (macro_map_.contains(k))
          continue;
        macro_map_.emplace(k, v);
      }
    }
    void remove(const std::wstring& macro) { macro_map_.erase(macro); }
    std::wstring replace(std::wstring src) {
      replaceTargetStr(&src);
      return src;
    }
    int replaceTargetStr(std::wstring* target) {
      int replace_cnt = 0;
      for (auto& [k, v] : macro_map_) {
        std::wstring macro = L"${" + k + L"}";
        auto i = target->find(macro);
        if (i == std::wstring::npos)
          continue;
        *target = target->replace(i, macro.size(), v);
        replace_cnt++;
      }
      return replace_cnt;
    }
  };
}
