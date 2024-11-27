module;
#include <toml++/toml.hpp>
export module ConfigLoader;
import std;
export class ConfigLoader {
private:

 public:
  ConfigLoader() = default;
  ~ConfigLoader() = default;
  void loadConfig();
  void saveConfig();
  void setConfig();
  void getConfig();
  void setConfigPath();
  void getConfigPath();

 private:
  std::string configPath;
  std::string config;
};