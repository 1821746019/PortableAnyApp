module;

#include <toml++/toml.hpp>
export module ConfigMgr;

// import my_converter.str;
import std;
import RegGuardConfig;
import my_converter.str;
export class ConfigMgr {
 public:
  enum RegModeEnum { Patch, Virtualize, Redirect };

 private:
  // 单例模式
  // ConfigMgr() {}
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  ConfigMgr(ConfigMgr&&) = delete;
  ConfigMgr& operator=(ConfigMgr&&) = delete;

  static std::string load(const std::wstring& config_path) {
    std::ifstream ifs(config_path);
    if (!ifs) {
      throw std::runtime_error("failed to open file");
    }

    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
  }
  toml::table config_;
  void initOtherConfig() {
    auto& regGuardConfig = RegGuardConfig::_ins_();
    for (auto& [k, v] : *config_["reg"].as_table()) {
      // std::print("{} => ", k.data());
      std::cout << config_ << '\n';
      // for (auto& [k1, v1] : *v.as_table()) {
      //   std::print("{} => {}", k1.data(),v1.value<std::string>()->data());
      // }
      // std::print("***\n");
      auto key = regGuardConfig.create(brv::strConvert(k));
      auto data = v.as_table()->at("redirect").value_exact<std::wstring>()->data();
      key->keyInfo->redirect = std::make_unique<std::wstring>(data);
    }
    int i = 0;
    // reg.create(L"");
  }
  ConfigMgr(const std::wstring& configPath) {
    initFinalConfig(configPath);
    initOtherConfig();
  }
  inline static std::unique_ptr<ConfigMgr> ins_{nullptr};
  std::optional<RegModeEnum> RegModeCached_;
  // 工厂模式
  static auto createIns(const std::wstring& configPath) {
    return std::unique_ptr<ConfigMgr>(new ConfigMgr(configPath));
  }

 public:
  void initFinalConfig(const std::wstring& configPath) { config_ = toml::parse(load(configPath)); }
  RegModeEnum RegMode() {
    if (RegModeCached_.has_value()) {
      return RegModeCached_.value();
    }
    std::string reg_mode = config_["RegMode"].value_or("Patch");
    std::unordered_map<std::string_view, RegModeEnum> map{
        {"Patch", Patch},
        {"Virtualize", Virtualize},
        {"Redirect", Redirect},
    };
    if (!map.contains(reg_mode)) {
      throw std::runtime_error("Couldn't find specified RegMode");
    }
    RegModeCached_ = map[reg_mode];
    return RegModeCached_.value();
  }
  static ConfigMgr& _ins_(const std::wstring& configPath) {
    if (!ins_) {
      ins_ = createIns(configPath);
    } else {
      throw std::runtime_error(
          "ConfigMgr already initialized, use _reinit_ if you want to reinit "
          "the ins"
      );
    }
    return *ins_.get();
  }
  static ConfigMgr& _reinit_(const std::wstring& configPath) {
    ins_ = createIns(configPath);
    return *ins_.get();
  }
  static ConfigMgr& _ins_() {
    if (!ins_) {
      throw std::runtime_error("ConfigMgr not initialized");
    }
    return *ins_.get();
  }
};
