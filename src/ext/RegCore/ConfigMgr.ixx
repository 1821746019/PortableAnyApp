module;

#include <toml++/toml.hpp>
export module ConfigMgr;

// import my_converter.str;
import std;

export class ConfigMgr {
 public:
  enum RegModeEnum { Patch, Virtual };

 private:
  ConfigMgr() = default;  // 私有构造函数，禁止外部直接创建实例
  ConfigMgr(const ConfigMgr&) = delete;
  ConfigMgr& operator=(const ConfigMgr&) = delete;
  static std::string load(const std::wstring& config_path) {
    std::ifstream ifs(config_path);
    if (!ifs) {
      throw std::runtime_error("failed to open file");
    }

    return std::string(std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>());
  }
  toml::table config_;
  ConfigMgr(const std::wstring& configPath) { initFinalConfig(configPath); }
  inline static std::unique_ptr<ConfigMgr> ins_{nullptr};
  std::optional<RegModeEnum> RegModeCached_;
  // 工厂模式
  static auto createIns(const std::wstring& configPath) {
    return std::unique_ptr<ConfigMgr>(new ConfigMgr(configPath));
  }

 public:
  void initFinalConfig(const std::wstring& configPath) {
    config_ = toml::parse(load(configPath));
  }
  RegModeEnum RegMode() {
    if (RegModeCached_.has_value()) {
      return RegModeCached_.value();
    }
    std::string reg_mode = config_["RegMode"].value_or("Patch");
    if (reg_mode == "Patch") {
      RegModeCached_ = Patch;
    } else if (reg_mode == "Virtual") {
      RegModeCached_ = Virtual;
    } else {
      throw std::runtime_error("RegMode not found");
    }
    return RegModeCached_.value();
  }
  static ConfigMgr& _ins_(const std::wstring& configPath) {
    if (!ins_) {
      ins_ = createIns(configPath);
    } else {
      throw std::runtime_error(
          "ConfigMgr already initialized, use _reinit_ if you want to reinit "
          "the ins");
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
